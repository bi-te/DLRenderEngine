#include <cstdio>
#include <iostream>
#include <thread>
#include <windows.h>    
#include <windowsx.h>  

#include "engine/math/Vec3.h"
#include "engine/math/Ray.h"
#include "engine/Timer.h"
#include "engine/Engine.h"
#include "engine/Window.h"
#include "engine/RayTracer.h"
#include "engine/SphereObject.h"

void generate_world();
void render(HWND hwnd);
void process_input(double dt);



int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    Timer timer(1./60);

    Engine& engine = Engine::instance();
    Screen& screen = engine.screen;

    Window window{ L"WindowClass", hInstance, WindowProc };
    window.create_window(L"Test21", screen.width, screen.height);
    window.show_window(nShowCmd);

    generate_world();

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
            engine.process_input(timer.get_frame_time());
            render(window.handle());
        }

        std::this_thread::yield();
    }
    
    return msg.wParam;
}

void render(HWND hwnd)
{
    Engine& engine = Engine::instance();
    RayTracer::process_rays(engine.world, engine.screen);

    Screen& screen = engine.screen;
    screen.update(hwnd);   
}

void generate_world()
{
    Engine& engine = Engine::instance();

    SphereObject sphere{{50, 50, 0}, 40};

    engine.world.add(sphere);

    sphere = { {150, 150, 0}, 40 };

    engine.world.add(sphere);
}
