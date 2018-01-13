#pragma once

#include <Utilities/UtilityTypes.h>
#include <Utilities/Math.h>

#include <Graphics/Window.h>

namespace pn {

namespace input {

// -------- ENUMERATIONS -----------

enum class key_state : unsigned char {
	PRESSED, 
	RELEASED,
	JUST_PRESSED, 
	JUST_RELEASED
};

enum class mouse_wheel_state : unsigned char {
	SCROLL_UP,
	SCROLL_DOWN,
	NO_CHANGE
};

enum input_key : unsigned char {
	LEFT_MOUSE		= VK_LBUTTON,
	RIGHT_MOUSE		= VK_RBUTTON,
	MIDDLE_MOUSE	= VK_MBUTTON,

	TILDE			= VK_OEM_3,
	TAB				= VK_TAB, 
	BACKSPACE		= VK_BACK, 
	SPACE			= VK_SPACE, 
	CAPS_LOCK		= VK_CAPITAL, 
	ENTER			= VK_RETURN,
	SHIFT			= VK_SHIFT, 
	CONTROL			= VK_CONTROL, 
	ALT				= VK_MENU, 
	ESCAPE			= VK_ESCAPE,

	LEFT			= VK_LEFT, 
	RIGHT			= VK_RIGHT, 
	UP				= VK_UP, 
	DOWN			= VK_DOWN,

	_0 = 0x30, _1, _2, _3, _4, _5, 
	_6, _7, _8, _9,

	A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, 
	N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

	NUMPAD_0 = VK_NUMPAD0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4, NUMPAD_5,
	NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9, 

	F1 = VK_F1, F2, F3, F4, F5, F6, F7, F8, F9,
	F10, F11, F12

};

// -------- TYPE DEFINITIONS ----------

struct mouse_pos_t {
	signed short x;
	signed short y;
};

struct input_state_t {
	key_state			keys[256];
	mouse_wheel_state	mouse_wheel;
	mouse_pos_t			mouse_pos;
};

// -------- FUNCTIONS --------------

void				InitInput();
void				InputOnEndOfFrame();

void				SetKeyState(const unsigned int vkey, const key_state state);
key_state			GetKeyState(const input_key vkey);

void				SetMousePos(const mouse_pos_t mouse_pos);
mouse_pos_t			GetMousePos();

void				SetMouseWheelState(const mouse_wheel_state state);
mouse_wheel_state	GetMouseWheelState();

void				AddInputCharacter(const unsigned char c);
const string&		GetInputCharacters();


} // namespace input

} // namespace pn