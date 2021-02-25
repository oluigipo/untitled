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
	glfwSwapInterval(0);
	
	int width = 1280;
	int height = 720;
	
	GLFWwindow* window = glfwCreateWindow(width, height, "game", NULL, NULL);
	if (!window) {
		printf("Couldn't initialize window.\nExiting.");
		glfwTerminate();
		return 1;
	}
	
	glfwMakeContextCurrent(window);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Could not load OpenGL functions.\nExiting.");
		glfwTerminate();
		return 1;
	}
	
	glfwSetWindowSizeCallback(window, glfwcallback_window_resize);
	
	game.apiWindow = window;
	game.window.width = (uint)width;
	game.window.height = (uint)height;
	game.targetFPS = args->fps;
	game.frameRate = 1000000 / args->fps;
	
	// Init some other things
	input_init();
	stbi_set_flip_vertically_on_load(true);
	texture_load_assets();
	random_init();
	
	// Initialize OpenGL things
	glfwcallback_window_resize(window, width, height);
	
	glEnable(GL_BLEND);
	// glEnable(GL_CULL_FACE);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	return 0;
}

void engine_begin_frame(void) {
	game.frameBegin = glfwGetTime();
	game.deltaTime = (f32)((game.frameBegin - game.lastFrame) * game.targetFPS);
	
	// NOTE(luigi): glfwPollEvents(void) is inside input_update(void).
	//              mouse input is 1 frame behind VS oldKeys not working.
	input_update();
}

void engine_end_frame(void) {
	++game.frameCount;
	glfwSwapBuffers(game.apiWindow);
	
	f64 frameEnd = glfwGetTime();
	uint frameDuration = (uint)((frameEnd - game.frameBegin) * 1000000);
	
	// V-Sync
	if (game.frameRate > frameDuration) {
		usleep(game.frameRate - frameDuration);
	}
}

void engine_deinit(void) {
	texture_free_assets();
	glfwTerminate();
}
