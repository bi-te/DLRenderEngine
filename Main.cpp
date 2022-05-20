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

    Window window{ L"WindowClass", hInstance};
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