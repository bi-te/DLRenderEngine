#pragma once

#include "Controller.h"
#include "Engine.h"

#include "includes/win.h"

LRESULT CALLBACK WindowProc(HWND hwnd,
                            UINT message, WPARAM wparam, LPARAM lparam);

class Window
{
    WNDCLASSEX wc;
    HWND window;


public:
    Window(LPCWSTR class_name, HINSTANCE hInstance);

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
