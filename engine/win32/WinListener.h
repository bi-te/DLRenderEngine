#pragma once

#include "win.h"

enum BUTTON { MUP, PRESSED, RELEASED, MDOWN };

enum Key
{
    NONE = 0x00, LMOUSE = 0x01, RMOUSE = 0x02,
    SHIFT = 0x10, CTRL = 0x11, ESCAPE = 0x1B, SPACE = 0x20, LEFT = 0x25, UP = 0x26, RIGHT = 0x27, DOWN = 0x28,
    A = 0x41, C = 0x43, D = 0x44, E = 0x45, F = 0x46, G = 0x47, I = 0x49, N = 0x4E,
    P = 0x50, Q = 0x51, R = 0x52, S = 0x53, W = 0x57,
    PLUS = 0xBB, MINUS = 0xBD
};

class IWinListener
{
public:
    virtual void OnResize(uint32_t width, uint32_t height) = 0;
    virtual void KeyEvent(Key key, bool status) = 0;
    virtual void MouseEvent(Key button, BUTTON status, uint32_t x_pos, uint32_t y_pos) = 0;
    virtual void MouseWheelEvent(uint32_t count) = 0;
    virtual void OnSizeMoved() = 0;
};
