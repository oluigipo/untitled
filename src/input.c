#include "headers/input.h"
#include "headers/opengl.h"
#include "headers/global.h"
#include <memory.h>

struct Keyboard keyboard = { 0 };
struct Mouse mouse = { 0 };
struct Gamepad gamepad = { .id = -1 };

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

internal void input_update_gamepad(void) {
	glfwGetGamepadState(gamepad.id, &gamepad.state);
	
	gamepad.state.axes[GPAD_AXIS_L2] = (gamepad.state.axes[GPAD_AXIS_L2] + 1.0f) * 0.5f;
	gamepad.state.axes[GPAD_AXIS_R2] = (gamepad.state.axes[GPAD_AXIS_R2] + 1.0f) * 0.5f;
	
	for (uint i = 0; i < GPAD_AXIS_COUNT; ++i) {
		if (fabsf(gamepad.state.axes[i]) < GPAD_DEADZONE)
			gamepad.state.axes[i] = 0.0f;
	}
	
	if (gamepad.state.axes[0] != 0.0f && gamepad.state.axes[1] != 0.0f)
		glm_vec2_normalize(gamepad.state.axes);
	
	if (gamepad.state.axes[2] != 0.0f && gamepad.state.axes[3] != 0.0f)
		glm_vec2_normalize(gamepad.state.axes + 2);
}

void input_init(void) {
	glfwSetKeyCallback(game.apiWindow, glfwcallback_key_update);
	glfwSetMouseButtonCallback(game.apiWindow, glfwcallback_mouse_update);
	glfwSetScrollCallback(game.apiWindow, glfwcallback_scroll_update);
}

void input_update(void) {
	memcpy(keyboard.oldKey, keyboard.key, sizeof(keyboard.key));
	memcpy(mouse.oldButton, mouse.button, sizeof(mouse.button));
	gamepad.oldState = gamepad.state;
	
	mouse.oldPos[0] = mouse.pos[0];
	mouse.oldPos[1] = mouse.pos[1];
	
	mouse.scroll = 0;
	
	glfwPollEvents();
	
	f64 x, y;
	glfwGetCursorPos(game.apiWindow, &x, &y);
	
	mouse.pos[0] = (f32)x;
	mouse.pos[1] = (f32)y;
	
	if (gamepad.id != -1 && !glfwJoystickPresent(gamepad.id))
		gamepad.id = -1;
	
	if (gamepad.id == -1) {
		gamepad.id = input_check_for_gamepad();
		
		debug_log("gamepad.id = %i\n", gamepad.id);
		
		if (gamepad.id != -1) {
			gamepad.name = glfwGetGamepadName(gamepad.id);
			debug_log("Gamepad name: %s\n", gamepad.name);
			
			input_update_gamepad();
		} else {
			input_map_to_gamepad();
		}
		
	} else {
		input_update_gamepad();
	}
}

void input_map_to_gamepad(void) {
	// TODO(luigi): convert input from the keyboard to gamepad. Higher density of relevant data.
	gamepad.state = (GLFWgamepadstate) { 0 };
	
	gamepad.state.axes[GPAD_AXIS_LX] = keyboard_is_down(GLFW_KEY_D) - keyboard_is_down(GLFW_KEY_A);
	gamepad.state.axes[GPAD_AXIS_LY] = keyboard_is_down(GLFW_KEY_S) - keyboard_is_down(GLFW_KEY_W);
	
	if (gamepad.state.axes[GPAD_AXIS_LX] != 0.0f && gamepad.state.axes[GPAD_AXIS_LY] != 0.0f)
		glm_vec2_normalize(gamepad.state.axes);
	
	gamepad.state.axes[GPAD_AXIS_RX] = keyboard_is_down(GLFW_KEY_Q) - keyboard_is_down(GLFW_KEY_E);
	gamepad.state.buttons[GPAD_BUTTON_A] = keyboard_is_down(GLFW_KEY_SPACE);
}

int input_check_for_gamepad(void) {
	int id = GLFW_JOYSTICK_1;
	
	while (id <= GLFW_JOYSTICK_LAST) {
		if (glfwJoystickPresent(id) && glfwJoystickIsGamepad(id))
			return id;
		++id;
	}
	
	return -1;
}


