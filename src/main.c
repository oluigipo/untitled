#define __CREATE_IMPL__
#include "std.h"
#include <time.h>
#include "memory.c"
#include "debug.c"

#define STB_IMAGE_IMPLEMENTATION
#ifdef TCC
#define STBI_NO_SIMD
#endif
#define STBI_MALLOC mem_alloc
#define STBI_REALLOC mem_realloc
#define STBI_FREE mem_free
#include "glad.c"
#include <glfw3.h>
#include <cglm/cglm.h>
#include <stb_image.h>

typedef uint (*__scene_t)(void);
typedef __scene_t Scene;

struct GameGlobalState {
	
	// Game Window
	GLFWwindow* apiWindow;
	struct GameWindowState {
		uint width, height;
	} window;
	
	// Game Scene
	Scene currentScene;
	
	// Frame time calculation
	u64 frameCount;
	f32 deltaTime;
	f32 targetFPS; // The desired FPS
	uint frameRate; // Desired time to spend on each frame (milliseconds)
	f64 lastFrame; // Time when the last frame started (seconds)
	f64 frameBegin; // Time when the current frame started (seconds)
	
	// Screen
	mat4 projection;
	
	// Global stacks
	uint framebufferStack[16]; // The first index should always be 0.
	uint framebufferStackSize; // The stack will init at 1.
	uint shaderStack[16]; //     /\ Same for all stacks.
	uint shaderStackSize;
	
	// Temporary Memory
	struct Arena frameArena; // Temporary memory for things that should live an entire frame.
} game = { 0 };

struct GameArgs {
	uint fps;
	usize arena;
};

typedef uint vec2u[2];
typedef int vec2i[2];

// Game Core
#include "input.c"
#include "file.c"
#include "shader.c"
#include "texture.c"
#include "random.c"
#include "text.c"

#include "engine.c"

// Scenes
#include "scenes/main_scene.c"

// Main Function
internal void parse_args(struct GameArgs* restrict args, uint argc, const char* restrict* restrict argv);

int main(int argc, char* argv[]) {
	struct GameArgs args;
	parse_args(&args, (uint)argc, (void*)argv);
	
	uint result = engine_init(&args);
	if (result != 0)
		return result;
	
	game.currentScene = scene_main;
	do {
		result = game.currentScene();
	} while (game.currentScene && !result);
	
	engine_deinit();
	return result;
}

internal u64 hash_of(const char* restrict str) {
	u64 hash = 2166136261ull;
	
	while (*str) {
		hash ^= (u64)*str++;
		hash *= 16777619ull;
	}
	
	return hash;
}

internal void parse_args(struct GameArgs* restrict args, uint argc, const char* restrict* restrict argv) {
	// Default values
	args->fps = 60;
	args->arena = 1024 * 1024;
	
	// Parse arguments
	for (uint i = 1; i < argc; ++i) {
		const char* restrict arg = argv[i];
		
		// Argument shall begin with a -
		if (arg[0] != '-') {
			continue;
		}
		
		u64 hash = hash_of(arg + 1); // ignore first character
		
		switch (hash) {
			// arena
			case 4533368378118350312ull: {
				++i;
				arg = argv[i];
				if (!arg) {
					printf("Missing value for argument '-arena'. Default to %zu.\n", args->arena);
					
					break;
				}
				
				usize s;
				sscanf(arg, "%zu", &s);
				
				if (s < args->arena) {
					printf("Argument for '-arena' shall be greater than the default value %zu. Ignoring flag.\n", args->arena);
					
					break;
				}
				
				args->arena = s;
			} break;
			
			// fps
			case 6524616317257075368ull: {
				++i;
				arg = argv[i];
				if (!arg) {
					printf("Missing value for argument '-fps'. Default to %u.\n", args->fps);
					
					break;
				}
				
				sscanf(arg, "%u", &args->fps);
			} break;
			
			// Unknown
			default: {
				printf("Ignoring argument '%s'\n", arg);
			} break;
		}
	}
}
