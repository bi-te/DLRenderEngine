#pragma once
#include <cstdint>

enum BUTTON{PRESSED, DOWN, RELEASED, UP};

struct Mouse
{
	int16_t prev_x, prev_y;
	int16_t x, y;

	BUTTON rmb{UP};
	BUTTON lmb{UP};

};

struct Keyboard
{
	bool up, down, left, right;
	bool exit;

	Keyboard() : up(false), down(false), left(false), right(false), exit(false)
	{
	}
};

struct InputState
{
	Keyboard keyboard;
	Mouse mouse;	
};