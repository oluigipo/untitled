internal void glfwcallback_window_resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	game.window.width = (uint)width;
	game.window.height = (uint)height;
	
	glm_mat4_identity(game.projection);
	glm_ortho(0.0f, (f32)width, (f32)height, 0.0f, -1.0f, 1.0f, game.projection);
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
	arena_init(&game.frameArena, args->arena);
	input_init();
	stbi_set_flip_vertically_on_load(true);
	texture_load_assets();
	random_init();
	
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
	arena_clear(&game.frameArena);
	
	debug({
			  static f64 sum = 0;
			  sum += glfwGetTime() - game.frameBegin;
			  
			  if (game.frameCount % 60 == 0) {
				  debug_print("Average Frame Time: %f ms\n", sum / FPS_DEFAULT * 1000);
				  sum = 0;
			  }
		  });
	
	game.lastFrame = game.frameBegin;
	
	glfwSwapBuffers(game.apiWindow);
}

void engine_deinit(void) {
	texture_free_assets();
	glfwTerminate();
}
