#pragma once
#include "types.h"
#include "opengl.h"
#include "memory.h"
#include "options.h"

#define FPS_DEFAULT 60

typedef uint (*__scene_t)(void);
typedef __scene_t Scene;

struct GameGlobalState {
	// Game Window
	GLFWwindow* apiWindow;
	struct GameWindowState {
		uint width, height;
	} window;
	b32 vsyncEnabled;
	i32 refreshRate;
	
	// NOTE(luigi): maybe we are going to support software rendering using MESA's software implementation of OpenGL? well, first we need to be able to bind GDI32.dll to mesa_opengl32.dll at runtime, which isn't possible at the moment.
	//u32 renderer; // 0 = normal, 1 = software (windows only)
	
	// Game Scene
	Scene currentScene;
	
	// Frame time calculation
	u64 frameCount;
	f64 deltaTime;
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
	Stack frameStack; // Temporary memory for things that should live a single frame.
};

struct GameArgs {
	usize mem;
	uint width;
	uint height;
	uint locale;
	b8 fullscreen;
	b8 novsync;
};

global struct GameGlobalState game;

