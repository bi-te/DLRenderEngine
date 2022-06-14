#include "Window.h"

#include <windowsx.h>


Window::Window(LPCWSTR class_name, HINSTANCE hInstance)
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

void Window::create_window(LPCWSTR name, LONG width, LONG height)
{
    RECT rect{ 0, 0, width, height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    window = CreateWindowEx(NULL, wc.lpszClassName, name, WS_OVERLAPPEDWINDOW,
        300, 300, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, wc.hInstance, NULL);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    InputState& is = Controller::input_state();
    ImageSettings& im = Controller::image_settings();
    
    int16_t x = GET_X_LPARAM(lParam);
    int16_t y = GET_Y_LPARAM(lParam);
    switch (message)
    {
    case WM_RBUTTONDOWN:
        im.global_illumination = GI_OFF;

        is.mouse.rmb = PRESSED;
        is.mouse.x = x;
        is.mouse.y = y;
        break;
    case WM_LBUTTONDOWN:
        im.global_illumination = GI_OFF;

        is.mouse.lmb = PRESSED;
        is.mouse.lmb_x = x;
        is.mouse.lmb_y = y;
        break;
    case WM_MOUSEMOVE:
        is.mouse.x = x;
        is.mouse.y = y;
        break;
    case WM_MOUSEWHEEL:
        is.mouse.wheel += GET_WHEEL_DELTA_WPARAM(wParam) / 120;

    case WM_RBUTTONUP:
        is.mouse.rmb = RELEASED;
        break;
    case WM_LBUTTONUP:
        is.mouse.lmb = RELEASED;
        break;

    case WM_KEYDOWN:
        im.global_illumination = GI_OFF;
        if(0x31 <= wParam && wParam <= 0x39)
        {
            Engine::instance().screen.set_shrink(wParam % 16);
        }
        is.keyboard.keys[wParam] = true;
        break;

    case WM_KEYUP:
        is.keyboard.keys[wParam] = false;
        break;

    case WM_SIZE:
        im.global_illumination = GI_OFF;

        Engine::instance().screen.init_resize(LOWORD(lParam), HIWORD(lParam));
        Engine::instance().camera.change_aspect(float(LOWORD(lParam)) / HIWORD(lParam));
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}