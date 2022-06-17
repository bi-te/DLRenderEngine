#include <cstdio>
#include <iostream>
#include <thread>
#include <windows.h>    
#include <windowsx.h>

#include "engine/Controller.h"
#include "engine/Timer.h"
#include "engine/Engine.h"
#include "engine/Window.h"

uint32_t ParallelExecutor::MAX_THREADS = max(1u, std::thread::hardware_concurrency());
uint32_t ParallelExecutor::HALF_THREADS = max(1u, std::thread::hardware_concurrency() / 2);

void initConsole()
{
    AllocConsole();
    FILE* dummy;
    auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
}

void render(HWND hwnd);
void render(HWND hwnd, ParallelExecutor& executor);

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    //initConsole();

    Timer timer(1.f/60.f);

    uint32_t numThreads = max(1u, max(ParallelExecutor::MAX_THREADS - 4u, ParallelExecutor::HALF_THREADS));
    ParallelExecutor executor(numThreads);

    Engine& engine = Engine::instance();
    Screen& screen = engine.screen;
    screen.init_resize(300, 180);
    screen.set_shrink(4);

    Window window{L"WindowClass", hInstance};
    window.create_window(L"Test21", screen.width(), screen.height());
    window.show_window(nShowCmd);

    engine.camera.set_perspective(to_radians(35.f), float(screen.width()) / screen.height(), 1.f, 500.f);

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
            controller.process_input(timer.time_passed());
            timer.advance_current();
            render(window.handle(), executor);
            //render(window.handle());
        }
        std::this_thread::yield();
    }
    
    return msg.wParam;
}

void render(HWND hwnd)
{
    Engine& engine = Engine::instance();
    Screen& screen = engine.screen;
    ImageSettings& im = Controller::image_settings();

    screen.resize();
    engine.scene.draw(screen, im, engine.camera);
    if (im.progressive_gi) im.gi_frame++;
	screen.update(hwnd);   
}

void render(HWND hwnd, ParallelExecutor& executor)
{
    Engine& engine = Engine::instance();
    Screen& screen = engine.screen;
    ImageSettings& im = Controller::image_settings();

    screen.resize();
    engine.scene.draw(screen, im, engine.camera, executor);
    if (im.progressive_gi) im.gi_frame++;
    screen.update(hwnd);
}