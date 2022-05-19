#include <cstdio>
#include <iostream>
#include <thread>
#include <windows.h>    
#include <windowsx.h>

#include "engine/Controller.h"
#include "engine/Timer.h"
#include "engine/Engine.h"
#include "engine/Window.h"

void initConsole()
{
    AllocConsole();
    FILE* dummy;
    auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
}

void render(HWND hwnd);

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    //initConsole();

    Timer timer(1.f/60);

    Engine& engine = Engine::instance();
    Screen& screen = engine.screen;
    screen.init_resize(800, 600);

    Window window{ L"WindowClass", hInstance, WindowProc };
    window.create_window(L"Test21", screen.width(), screen.height());
    window.show_window(nShowCmd);

    engine.camera.set_perspective(to_radians(30), float(screen.width()) / screen.height(), 1, 300);

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
            timer.restart();
            render(window.handle());
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
        is.mouse.x = x;
        is.mouse.y = y;
        break;
    case WM_LBUTTONDOWN:
        is.mouse.lmb = PRESSED;
        is.mouse.lmb_x = x;
        is.mouse.lmb_y = y;
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
	    case VK_CONTROL:
            is.keyboard.down = true;
            break;

		case VK_LEFT:
            is.keyboard.yawleft = true;
            break;
		case VK_RIGHT:
			is.keyboard.yawright = true;
            break;
        case VK_UP:
            is.keyboard.pitchup = true;
            break;
        case VK_DOWN:
            is.keyboard.pitchdown = true;
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
	    case VK_CONTROL:
            is.keyboard.down = false;
	        break;

        case VK_LEFT:
            is.keyboard.yawleft = false;
            break;
        case VK_RIGHT:
            is.keyboard.yawright = false;
            break;
        case VK_UP:
            is.keyboard.pitchup = false;
            break;
        case VK_DOWN:
            is.keyboard.pitchdown = false;
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