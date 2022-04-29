#include <cstdio>
#include <iostream>
#include <thread>
#include <windows.h>    
#include <windowsx.h>  

#include"vec3.h"
#include"ray.h"
#include "sphere.h"
#include "timer.h"
#include "engine.h"


#define BUTTON_W 0x57
#define BUTTON_A 0x41
#define BUTTON_S 0x53
#define BUTTON_D 0x44


void render(HWND hwnd);
void process_keyboard();

LRESULT CALLBACK WindowProc(HWND hwnd,
    UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    timer timer(1./30);
    
    WNDCLASSEX wc;

    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"WindowClass";

    RegisterClassEx(&wc);

    screen& screen = engine::instance().screen;
    RECT rect{ 0, 0, screen.width, screen.height };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    HWND window = CreateWindowEx(NULL, L"WindowClass", L"Test21", WS_OVERLAPPEDWINDOW,
        300, 300, rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, hInstance, NULL);

    ShowWindow(window, nShowCmd);

    MSG msg;

    timer.start();
    while(true)
    {
	    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	    {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) goto exit_loop;;
	    }

        
        while(timer.frame_time_check())
        {
            process_keyboard();
            render(window);
        }
        std::this_thread::yield();
    }
    exit_loop:;
    return msg.wParam;
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    mouse& mouse = engine::instance().mouse;
    sphere& sp = engine::instance().r_sphere;
    int32_t x = GET_X_LPARAM(lParam);
    int32_t y = GET_Y_LPARAM(lParam);

    switch (message)
    {
    case WM_RBUTTONDOWN:
        
        if(engine::check_collision(ray{{x, y, 0}, {0, 0, 1}}, sp))
        {
            mouse.mx = x;
            mouse.my = y;
            mouse.can_move = true;
        }        
        break;
    case WM_MOUSEMOVE:
        if(mouse.can_move)
        {
            sp.set_center(sp.get_center() + vec3{ x- mouse.mx, y- mouse.my, 0 });
            mouse.mx = x;
            mouse.my = y;
        }
        break;
    case WM_RBUTTONUP:
        mouse.can_move = false;
        break;        
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

void render(HWND hwnd)
{
    engine& engine = engine::instance();
    engine.process_rays();

    screen& screen = engine.screen;

    HDC hdc= GetDC(hwnd);

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(BITMAPINFO));

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = screen.width;
    bmi.bmiHeader.biHeight = -screen.height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;

    SetDIBitsToDevice(hdc, 0, 0, screen.width, screen.height, 0, 0, 0, screen.height,
        screen.buffer.data(), &bmi, DIB_RGB_COLORS);
}

void process_keyboard()
{
    sphere& sp = engine::instance().r_sphere;
    if (GetKeyState(VK_ESCAPE) & 0x8000) {
        PostQuitMessage(0);
        return ;
    }
    if (GetKeyState(BUTTON_W) & 0x8000) sp.set_center(sp.get_center() + vec3{ 0, -10, 0 });//W
    if (GetKeyState(BUTTON_A) & 0x8000) sp.set_center(sp.get_center() + vec3{ -10, 0, 0 });//A
    if (GetKeyState(BUTTON_S) & 0x8000) sp.set_center(sp.get_center() + vec3{ 0, 10, 0 });//S
    if (GetKeyState(BUTTON_D) & 0x8000) sp.set_center(sp.get_center() + vec3{ 10, 0, 0 });//D
}
