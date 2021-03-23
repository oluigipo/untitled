#include "headers/all.h"
#include <stb_image.h>

internal void glfwcallback_window_resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	game.window.width = (uint)width;
	game.window.height = (uint)height;
	
	glm_mat4_identity(game.projection);
	glm_ortho((f32)-width/2, (f32)width/2, (f32)height/2, (f32)-height/2, -1.0f, 1.0f, game.projection);
}

uint engine_init(const struct GameArgs* restrict args) {
	if (!glfwInit()) {
		printf("Failed to initialize GLFW.\nExiting.");
		return 1;
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(args->width, args->height, "game", NULL, NULL);
	if (!window) {
		printf("Couldn't initialize window.\nExiting.");
		glfwTerminate();
		return 1;
	}
	
	glfwMakeContextCurrent(window);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glfwSwapInterval(!args->novsync);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, mode->width / 2 - args->width / 2, mode->height / 2 - args->height / 2);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Could not load OpenGL functions.\nExiting.");
		glfwTerminate();
		return 1;
	}
	
	glfwSetWindowSizeCallback(window, glfwcallback_window_resize);
	
	game.apiWindow = window;
	game.window.width = args->width;
	game.window.height = args->height;
	game.lastFrame = glfwGetTime();
	game.framebufferStackSize = 1;
	game.shaderStackSize = 1;
	
	// Init some other things
	stack_init(&game.frameStack, args->mem);
	input_init();
	stbi_set_flip_vertically_on_load(true);
	texture_load_assets();
	random_init();
	locale_init();
	partmgr_setup_rendering();
	sound_init();
	
	//debug(random_test());
	
	// Initialize OpenGL things
	glfwcallback_window_resize(window, args->width, args->height);
	
	glEnable(GL_BLEND);
	// glEnable(GL_CULL_FACE);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	return 0;
}

void engine_begin_frame(void) {
	game.frameBegin = glfwGetTime();
	game.deltaTime = (game.frameBegin - game.lastFrame) * FPS_DEFAULT;
	
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
	
	debug({
			  static f64 sumTotal = 0;
			  static f64 sumWasted = 0;
			  
			  f64 now = glfwGetTime();
			  sumTotal += now - game.lastFrame;
			  sumWasted += now - game.frameBegin;
			  
			  if (sumTotal > 1.0) {
				  debug_print("Average Frame Time: %f ms\n", sumWasted / FPS_DEFAULT * 1000);
				  sumTotal = 0;
				  sumWasted = 0;
			  }
		  });
	
	game.lastFrame = game.frameBegin;
	
	// Vsync and swap chain
	glfwSwapBuffers(game.apiWindow);
}

void engine_deinit(void) {
	texture_free_assets();
	locale_deinit();
	sound_deinit();
	glfwTerminate();
}
