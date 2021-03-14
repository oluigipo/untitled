#include "headers/input.h"
#include "headers/opengl.h"
#include "headers/global.h"
#include <memory.h>

struct Keyboard keyboard = { 0 };
struct Mouse mouse = { 0 };
struct Gamepad gamepad = { 0 };

internal void glfwcallback_key_update(GLFWwindow* window, int key, int scancode, int action, int mods) {
	keyboard.key[key] = (action != 0);
	keyboard.lastKey = key;
}

internal void glfwcallback_mouse_update(GLFWwindow* window, int button, int action, int mods) {
	mouse.button[button] = (action != 0);
}

internal void glfwcallback_scroll_update(GLFWwindow* window, f64 xaxis, f64 yaxis) {
	mouse.scroll = (int)yaxis;
}

void input_init(void) {
	glfwSetKeyCallback(game.apiWindow, glfwcallback_key_update);
	glfwSetMouseButtonCallback(game.apiWindow, glfwcallback_mouse_update);
	glfwSetScrollCallback(game.apiWindow, glfwcallback_scroll_update);
}

void input_update(void) {
	memcpy(keyboard.oldKey, keyboard.key, sizeof(keyboard.key));
	memcpy(mouse.oldButton, mouse.button, sizeof(mouse.button));
	
	mouse.oldPos[0] = mouse.pos[0];
	mouse.oldPos[1] = mouse.pos[1];
	
	mouse.scroll = 0;
	
	glfwPollEvents();
	
	f64 x, y;
	glfwGetCursorPos(game.apiWindow, &x, &y);
	
	mouse.pos[0] = (f32)x;
	mouse.pos[1] = (f32)y;
}

