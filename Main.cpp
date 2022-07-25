#include <cstdio>
#include <iostream>
#include <thread>

#include "engine/includes/win.h"

#include "engine/Controller.h"
#include "engine/Timer.h"
#include "engine/Engine.h"
#include "engine/Window.h"
#include "imgui/ImGuiManager.h"


void initConsole()
{
    AllocConsole();
    FILE* dummy;
    auto s = freopen_s(&dummy, "CONOUT$", "w", stdout); // stdout will print to the newly created console
}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    //initConsole();

    Timer timer(1.f / 60.f);
    uint32_t width = 1024, height = 768;

    ImGuiManager::init_context();
   
    Window window{L"WindowClass", hInstance};
    window.create_window(L"Test21", width, height);

    Engine::init(window.handle());
    Engine& engine = Engine::instance();
    engine.camera.set_perspective(to_radians(55.f), float(width) / height, 0.1f, 400.f);

    Controller controller{engine.scene, engine.camera, engine.renderer};
    controller.init_scene();

    ImGuiManager::init_render(window.handle(),
        Direct3D::globals().device5.Get(), Direct3D::globals().context4.Get());

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
            if (ImGuiManager::active())
            {
                ImGuiManager::new_frame();
                controller.process_gui_input();
            }
                
            controller.process_input(timer.time_passed());
            timer.advance_current();

            if (!IsIconic(window.handle()))
                engine.scene.draw(engine.camera, engine.renderer);
        }




        std::this_thread::yield();
    }

    ImGuiManager::reset();

    engine.scene.reset_objects_buffers();
    Engine::reset();

    return msg.wParam;
}


