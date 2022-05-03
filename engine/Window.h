#pragma once

#include <string>
#include <windows.h>

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
	Window(LPCWSTR class_name, HINSTANCE hInstance, WNDPROC WindowProc)
	{

        ZeroMemory(&wc, sizeof(WNDCLASSEX));

        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
        wc.lpszClassName = class_name;

        RegisterClassEx(&wc);
	}

    HWND handle() const
    {
	    return window;
    }

    void create_window(LPCWSTR name, LONG width, LONG height)
	{
        RECT rect{ 0, 0, width, height };
        AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

        window = CreateWindowEx(NULL, wc.lpszClassName, name, WS_OVERLAPPEDWINDOW,
            300, 300, rect.right - rect.left, rect.bottom - rect.top,
            NULL, NULL, wc.hInstance, NULL);
	}

    void show_window(int nCmdShow)
	{
        ShowWindow(window, nCmdShow);
	}


};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    InputState& is = Engine::instance().input_state;
    int16_t x = GET_X_LPARAM(lParam);
    int16_t y = GET_Y_LPARAM(lParam);
    switch (message)
    {
    case WM_RBUTTONDOWN:
        is.mouse.rmb = PRESSED;
        break;

    case WM_MOUSEMOVE:
        is.mouse.x = x;
        is.mouse.y = y;
        break;

    case WM_RBUTTONUP:
        is.mouse.rmb = RELEASED;


    case WM_KEYDOWN:
        switch (wParam)
        {
        case BUTTON_W:
            is.keyboard.up = true;
            break;
        case BUTTON_A:
            is.keyboard.left = true;
            break;
        case BUTTON_S:
            is.keyboard.down = true;
            break;
        case BUTTON_D:
            is.keyboard.right = true;
            break;
        case VK_ESCAPE:
            is.keyboard.exit = true;
            break;
        }
        break;

    case WM_KEYUP:
        switch (wParam)
        {
        case BUTTON_W:
            is.keyboard.up = false;
            break;
        case BUTTON_A:
            is.keyboard.left = false;
            break;
        case BUTTON_S:
            is.keyboard.down = false;
            break;
        case BUTTON_D:
            is.keyboard.right = false;
            break;
        }
        break;

    case WM_SIZE:
        Engine::instance().screen.init_resize(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}