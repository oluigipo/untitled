#pragma once

#include "types.h"
#include "opengl.h"
#include "memory.h"

#define FPS_DEFAULT 60

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
	Arena frameArena; // Temporary memory for things that should live an entire frame.
};

struct GameArgs {
	usize arena;
	uint width;
	uint height;
	b8 fullscreen;
	b8 novsync;
};

global struct GameGlobalState game;

