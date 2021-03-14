#pragma once

#include "types.h"
#include "opengl.h"

#define keyboard_is_pressed(_key) (keyboard.key[_key] && !keyboard.oldKey[_key])
#define keyboard_is_down(_key) (keyboard.key[_key])
#define keyboard_is_released(_key) (!keyboard.key[_key] && keyboard.oldKey[_key])
#define keyboard_is_up(_key) (!keyboard.key[_key])

enum {
	INPUT_KEYCOUNT = GLFW_KEY_LAST,
	INPUT_BUTTONCOUNT = GLFW_MOUSE_BUTTON_LAST
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
	int dummy;
	// TODO
};

global struct Keyboard keyboard;
global struct Mouse mouse;
global struct Gamepad gamepad;

void input_init(void);
void input_update(void);
