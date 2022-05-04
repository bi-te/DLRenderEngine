#pragma once

#include <string>
#include <windows.h>

#include "Controller.h"
#include "Engine.h"

#define BUTTON_W 0x57
#define BUTTON_A 0x41
#define BUTTON_S 0x53
#define BUTTON_D 0x44

LRESULT CALLBACK WindowProc(HWND hwnd,
    UINT message, WPARAM wparam, LPARAM lparam);

class Window
{
    WNDCLASSEX wc;
    HWND window;
public:
    Window(LPCWSTR class_name, HINSTANCE hInstance, WNDPROC WindowProc);

    HWND handle() const
    {
	    return window;
    }

    void create_window(LPCWSTR name, LONG width, LONG height);

    void show_window(int nCmdShow)
	{
        ShowWindow(window, nCmdShow);
	}


};
