#include "headers/all.h"
#include <stb_image.h>

#if defined(OS_WINDOWS) && !(defined(TCC) || defined(MSVC))
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3native.h>
#endif

internal void game_update_projection_matrix(void) {
	glm_mat4_identity(game.projection);
	glm_scale(game.projection, (vec3) { 1.0f / (game.framebuffer.tex.size[0]/2.0f), 1.0f / -(game.framebuffer.tex.size[1]/2.0f) });
	//glm_ortho(-(f32)game.framebuffer.tex.size[0]/2, (f32)game.framebuffer.tex.size[0]/2, (f32)game.framebuffer.tex.size[1]/2, -(f32)game.framebuffer.tex.size[1]/2, -1.0f, 1.0f, game.projection);
}

internal void glfwcallback_window_resize(GLFWwindow* window, int width, int height) {
	debug_log("Window Resized: %ix%i\n", width, height);
	
	game.window.width = (uint)width;
	game.window.height = (uint)height;
	
	framebuffer_resize(&game.framebuffer, (vec2u) { (uint)width, (uint)height });
	if (game.framebufferStackSize == 1) {
		glViewport(0, 0, game.framebuffer.tex.size[0], game.framebuffer.tex.size[1]);
	}
	
	game_update_projection_matrix();
}

internal void glfwcallback_error_handler(int code, const char* name) {
	debug_error("GLFW Error %i: %s\n", code, name);
}

uint engine_init(const struct GameArgs* restrict args) {
	// Load options
	options_load();
	
	if (args->width) game.window.width = args->width;
	if (args->height) game.window.height = args->height;
	if (args->novsync) game.vsyncEnabled = false;
	
#if defined(OS_WINDOWS) && !(defined(TCC) || defined(MSVC))
	// NOTE(luigi): this is a modification I made on GLFW.
	// NOTE(luigi): we won't use it. See 'headers/global.h' for context.
	
	//if (game.renderer != 0) // if should use software rendering
	//glfwSetWin32LibraryName("mesa_opengl32.dll");
#endif
	
	// Load GLFW
	if (unlikely(!glfwInit())) {
		printf("Failed to initialize GLFW.\nExiting.\n");
		return 1;
	}
	
	glfwSetErrorCallback(glfwcallback_error_handler);
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, false);
	
	GLFWwindow* window = glfwCreateWindow(game.window.width, game.window.height, "game", NULL, NULL);
	if (!window) {
		debug_error("Couldn't initialize window.\nExiting.\n");
		glfwTerminate();
		return 1;
	}
	
	glfwMakeContextCurrent(window);
	
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, mode->width / 2 - game.window.width / 2, mode->height / 2 - game.window.height / 2);
	game.refreshRate = mode->refreshRate;
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		debug_error("Could not load OpenGL functions.\nExiting.\n");
		glfwDestroyWindow(window);
		glfwTerminate();
		return 1;
	}
	
	glfwSetWindowSizeCallback(window, glfwcallback_window_resize);
	
	game.apiWindow = window;
	game.lastFrame = glfwGetTime();
	game.shaderStackSize = 1;
	
	// Init some other things
	glfwSwapInterval(game.vsyncEnabled);
	stack_init(&game.frameStack, args->mem);
	input_init();
	stbi_set_flip_vertically_on_load(true);
	texture_load_assets();
	random_init();
	locale_init();
	partmgr_setup_rendering();
	sound_init();
	
	currentLocale = args->locale;
	
	//debug(random_test());
	
	// Initialize OpenGL things
	glEnable(GL_BLEND);
	// glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	framebuffer_init(&game.framebuffer, (vec2u) { game.window.width, game.window.height });
	framebuffer_bind(&game.framebuffer);
	
	game_update_projection_matrix();
	
	game.fbShader = shader_load("res/window");
	game.fbShaderTex = shader_uniform(game.fbShader, "uTexture");
	
	f32 vertices[] = {
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f
	};
	
	glGenBuffers(1, &game.fbVBO);
	glBindBuffer(GL_ARRAY_BUFFER, game.fbVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
	
	glGenVertexArrays(1, &game.fbVAO);
	glBindVertexArray(game.fbVAO);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(f32) * 2, (void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return 0;
}

void engine_begin_frame(void) {
	game.frameBegin = glfwGetTime();
	game.deltaTime = (game.frameBegin - game.lastFrame) * FPS_DEFAULT;
	
	glBindFramebuffer(GL_FRAMEBUFFER, game.framebuffer.id);
	glViewport(0, 0, game.framebuffer.tex.size[0], game.framebuffer.tex.size[1]);
	
	// NOTE(luigi): glfwPollEvents(void) is inside input_update(void).
	//              mouse input is 1 frame behind VS oldKeys not working.
	input_update();
}

void engine_end_frame(void) {
	++game.frameCount;
	
	// If 80% or more of it was used, double it's size, just to be safe.
	if ((usize)game.frameStack.header - (usize)game.frameStack.buffer >= game.frameStack.size / 5 * 4) {
		usize desiredSize = game.frameStack.size * 2;
		
		stack_deinit(&game.frameStack);
		stack_init(&game.frameStack, desiredSize);
	} else {
		stack_clear(&game.frameStack);
	}
	
	// Draw framebuffer to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, game.window.width, game.window.height);
	
	shader_bind(game.fbShader);
	glUniform1i(game.fbShaderTex, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, game.framebuffer.tex.id);
	
	glBindVertexArray(game.fbVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	
	shader_unbind();
	
	// Debug things
	debug({
			  static f64 sumTotal = 0;
			  static f64 sumWasted = 0;
			  
			  f64 now = glfwGetTime();
			  sumTotal += now - game.lastFrame;
			  sumWasted += now - game.frameBegin;
			  
			  if (sumTotal >= 1.0) {
				  debug_print("Average Frame Time: %f ms\n", sumWasted / FPS_DEFAULT * 1000);
				  sumTotal -= 1;
				  sumWasted = 0;
			  }
		  });
	
	// Vsync and swap chain
	game.lastFrame = game.frameBegin;
	glfwSwapBuffers(game.apiWindow);
}

void engine_deinit(void) {
	game.framebufferStackSize = 0;
	framebuffer_deinit(&game.framebuffer);
	options_save();
	texture_free_assets();
	locale_deinit();
	sound_deinit();
	glfwTerminate();
}
