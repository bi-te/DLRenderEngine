#include <cstdio>
#include <iostream>
#include <thread>
#include <windows.h>    
#include <windowsx.h>

#include "engine/Controller.h"
#include "engine/math/Vec3.h"
#include "engine/Timer.h"
#include "engine/Engine.h"
#include "engine/Window.h"

void render(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    Timer timer(1./60);

    Engine& engine = Engine::instance();
    Screen& screen = engine.screen;
    screen.init_resize(800, 600);

    Window window{ L"WindowClass", hInstance, WindowProc };
    window.create_window(L"Test21", screen.width(), screen.height());
    window.show_window(nShowCmd);

    engine.camera.set_perspective(to_radians(30), float(screen.width()) / screen.height(), 1, 100);

    Controller controller{engine.scene, engine.camera};
    controller.init_scene();

    MSG msg;

    timer.start();
    while(true)
    {
	    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	    {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT) break;
	    }
        if (msg.message == WM_QUIT) break;


        if(timer.frame_time_check())
        {
            controller.process_input(timer.get_dt());
            render(window.handle());
            timer.restart();
        }

        std::this_thread::yield();
    }
    
    return msg.wParam;
}

void render(HWND hwnd)
{
    Engine& engine = Engine::instance();
    Screen& screen = engine.screen;
    screen.resize();

    engine.scene.draw(screen,engine.camera);
    screen.update(hwnd);   
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    InputState& is = Controller::input_state();
    int16_t x = GET_X_LPARAM(lParam);
    int16_t y = GET_Y_LPARAM(lParam);
    switch (message)
    {
    case WM_RBUTTONDOWN:
        is.mouse.rmb = PRESSED;
        break;
    case WM_LBUTTONDOWN:
        is.mouse.lmb = PRESSED;
        break;

    case WM_MOUSEMOVE:
        is.mouse.x = x;
        is.mouse.y = y;
        break;

    case WM_RBUTTONUP:
        is.mouse.rmb = RELEASED;
        break;
    case WM_LBUTTONUP:
        is.mouse.lmb = RELEASED;
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case BUTTON_W:
            is.keyboard.forward = true;
            break;
        case BUTTON_A:
            is.keyboard.left = true;
            break;
        case BUTTON_S:
            is.keyboard.backward = true;
            break;
        case BUTTON_D:
            is.keyboard.right = true;
            break;
        case BUTTON_C :
            is.keyboard.down = true;
            break;
        case VK_SPACE:
            is.keyboard.up = true;
            break;
        case BUTTON_Q:
            is.keyboard.lroll = true;
            break;
        case BUTTON_E:
            is.keyboard.rroll = true;
            break;
        case VK_ESCAPE:
            is.keyboard.exit = true;
            break;

        case VK_LEFT:
            is.keyboard.ar_left = true;
            break;
        case VK_RIGHT:
            is.keyboard.ar_right = true;
            break;
        case VK_UP:
            is.keyboard.ar_up = true;
            break;
        case VK_DOWN:
            is.keyboard.ar_down = true;
            break;
        }
        break;

    case WM_KEYUP:
        switch (wParam)
        {
        case BUTTON_W:
            is.keyboard.forward = false;
            break;
        case BUTTON_A:
            is.keyboard.left = false;
            break;
        case BUTTON_S:
            is.keyboard.backward = false;
            break;
        case BUTTON_D:
            is.keyboard.right = false;
            break;
        case BUTTON_C:
            is.keyboard.down = false;
            break;
        case VK_SPACE:
            is.keyboard.up = false;
            break;
        case BUTTON_Q:
            is.keyboard.lroll = false;
            break;
        case BUTTON_E:
            is.keyboard.rroll = false;
            break;

        case VK_LEFT:
            is.keyboard.ar_left = false;
            break;
        case VK_RIGHT:
            is.keyboard.ar_right = false;
            break;
        case VK_UP:
            is.keyboard.ar_up = false;
            break;
        case VK_DOWN:
            is.keyboard.ar_down = false;
            break;
        }
        break;

    case WM_SIZE:
        Engine::instance().screen.init_resize(LOWORD(lParam), HIWORD(lParam));
        Engine::instance().camera.change_aspect(float(LOWORD(lParam)) / HIWORD(lParam));
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}