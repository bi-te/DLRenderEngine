#include <cstdio>
#include <iostream>
#include <thread>

#include "engine/win32/win.h"

#include "engine/Controller.h"
#include "engine/Timer.h"
#include "engine/Engine.h"
#include "win32/Window.h"
#include "imgui/ImGuiManager.h"


void initConsole()
{
    AllocConsole();
    FILE* dummy;
    auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
}

void render(const Camera& camera);

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    initConsole();

    Timer timer(1.f / 60.f);
    uint32_t width = 1366, height = 768;

    ImGuiManager::init_context();
    Engine::init();

    Window window{L"WindowClass", hInstance};
    window.create_window(L"Test21", width, height);

    Controller controller{window};
    controller.camera.set_perspective(rad(55.f), float(width) / height, 0.1f, 400.f);
    controller.init_scene();
    window.listeners.push_back(&controller);

    ImGuiManager::init_render(window.handle(), Direct3D::instance().device5.Get(), Direct3D::instance().context4.Get());

    MSG msg;
    window.show_window(nShowCmd);
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

            if (ImGuiManager::active())
            {
                ImGuiManager::new_frame();
                controller.process_gui_input();
            }

            if (!IsIconic(window.handle()))
                controller.render();
        }
        
        std::this_thread::yield();
    }

    ImGuiManager::reset();

    controller.render_reset();
    Engine::reset();

    return msg.wParam;
}

