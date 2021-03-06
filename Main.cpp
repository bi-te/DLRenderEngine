#include <cstdio>
#include <iostream>
#include <thread>

#include "engine/includes/win.h"   

#include "engine/render/Renderer.h"

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

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd)
{
    //initConsole();

    Timer timer(1.f/60.f);

    Engine& engine = Engine::instance();
    uint32_t width = 800, height = 600;

    Window window{L"WindowClass", hInstance};
    window.create_window(L"Test21", width, height);
    window.show_window(nShowCmd);

    engine.camera.set_perspective(to_radians(35.f), float(width) / height, 1.f, 500.f);

    Controller controller{engine.scene, engine.camera};
    controller.init_scene();

    MSG msg;
    timer.start();

    engine.renderer.init_swap_chain(window.handle());
    engine.renderer.init_render_target_view();
    engine.renderer.update_vertex_shader(L"shaders/vertex.hlsl", "main");
    engine.renderer.update_pixel_shader(L"shaders/pixel.hlsl", "main");
    engine.renderer.create_input_layout();
    engine.scene.init_buffers();

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
            //controller.process_input(timer.time_passed());
            timer.advance_current();

            if(!IsIconic(window.handle()))
                engine.renderer.draw(engine.scene);
        }
        std::this_thread::yield();
    }

    engine.renderer.clear();
    engine.scene.vertexBuffer.Reset();
    Direct3D::globals().clear();

    return msg.wParam;
}


