#pragma once

#include "types.h"
#include "opengl.h"

#define keyboard_is_pressed(_key) (keyboard.key[_key] && !keyboard.oldKey[_key])
#define keyboard_is_down(_key) (keyboard.key[_key])
#define keyboard_is_released(_key) (!keyboard.key[_key] && keyboard.oldKey[_key])
#define keyboard_is_up(_key) (!keyboard.key[_key])
#define gamepad_is_pressed(btn) (gamepad.state.buttons[btn] && !gamepad.oldState.buttons[btn])
#define gamepad_is_down(btn) (gamepad.state.buttons[btn])
#define gamepad_is_released(btn) (!gamepad.state.buttons[btn] && gamepad.oldState.buttons[btn])
#define gamepad_is_up(btn) (!gamepad.state.buttons[btn])
#define gamepad_axis(a) (gamepad.state.axes[a])
#define gamepad_axis_vec2(a) ((vec2) { gamepad_axis(a##X), gamepad_axis(a##Y) })

#define GPAD_DEADZONE 0.05f

enum {
	INPUT_KEYCOUNT = GLFW_KEY_LAST,
	INPUT_BUTTONCOUNT = GLFW_MOUSE_BUTTON_LAST
};

enum {
	GPAD_BUTTON_A = GLFW_GAMEPAD_BUTTON_A,
	GPAD_BUTTON_B = GLFW_GAMEPAD_BUTTON_B,
	GPAD_BUTTON_X = GLFW_GAMEPAD_BUTTON_X,
	GPAD_BUTTON_Y = GLFW_GAMEPAD_BUTTON_Y,
	GPAD_BUTTON_L1 = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
	GPAD_BUTTON_R1 = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
	GPAD_BUTTON_BACK = GLFW_GAMEPAD_BUTTON_BACK,
	GPAD_BUTTON_START = GLFW_GAMEPAD_BUTTON_START,
	GPAD_BUTTON_L3 = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
	GPAD_BUTTON_R3 = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
	GPAD_BUTTON_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP,
	GPAD_BUTTON_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
	GPAD_BUTTON_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
	GPAD_BUTTON_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT,
	GPAD_BUTTON_COUNT = GLFW_GAMEPAD_BUTTON_LAST+1,
	
	GPAD_AXIS_LX = GLFW_GAMEPAD_AXIS_LEFT_X,
	GPAD_AXIS_LY = GLFW_GAMEPAD_AXIS_LEFT_Y,
	GPAD_AXIS_RX = GLFW_GAMEPAD_AXIS_RIGHT_X,
	GPAD_AXIS_RY = GLFW_GAMEPAD_AXIS_RIGHT_Y,
	GPAD_AXIS_L2 = GLFW_GAMEPAD_AXIS_LEFT_TRIGGER,
	GPAD_AXIS_R2 = GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER,
	GPAD_AXIS_COUNT = GLFW_GAMEPAD_AXIS_LAST+1,
};

struct Keyboard {
	b8 key[INPUT_KEYCOUNT];
	b8 oldKey[INPUT_KEYCOUNT];
	
	int lastKey;
};

struct Mouse {
	b8 button[INPUT_BUTTONCOUNT];
	b8 oldButton[INPUT_BUTTONCOUNT];
	int scroll;
	
	vec2 pos;
	vec2 oldPos;
};

struct Gamepad {
	GLFWgamepadstate state;
	GLFWgamepadstate oldState;
	
	int id;
	const char* name;
};

global struct Keyboard keyboard;
global struct Mouse mouse;
global struct Gamepad gamepad;

void input_init(void);
void input_update(void);
void input_map_to_gamepad(void);
int input_check_for_gamepad(void);

