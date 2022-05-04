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

    Window window{ L"WindowClass", hInstance, WindowProc };
    window.create_window(L"Test21", screen.width(), screen.height());
    window.show_window(nShowCmd);

    Controller controller{engine.scene};
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
            controller.process_input(timer.get_frame_time());
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

    engine.scene.draw(screen);
    screen.update(hwnd);   
}

