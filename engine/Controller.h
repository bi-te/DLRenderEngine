#pragma once
#include <cstdint>
#include <iostream>

#include "Scene.h"
#include "math/math.h"
#include "win32/Window.h"
#include "win32/WinListener.h"

struct Mouse
{
    int32_t lmb_x, lmb_y;
    int32_t prev_x, prev_y;
    int32_t x, y;
    int32_t wheel;

    BUTTON rmb;
    BUTTON lmb;

};

struct Keyboard
{
    const uint8_t key_number = 254;
    std::vector<bool> keys;

    Keyboard()
    {
        keys.resize(key_number + 1);
    }
};

struct InputState
{
    Mouse mouse;
    Keyboard keyboard;
};

class  Controller: public IWinListener
{
    const float movement_speed = 20.f,
				shift = 5.f, dspeed = 1.1f,
				rotation_speed = 1.f,
				rotation_speed_mouse = 2.f * PI;

    IntersectionQuery record;

    Scene& scene;
    Window& window;
public:
    explicit Controller(Scene& scene, Window& window): scene(scene), window(window)
    {
    }

    InputState is{};
    

    void move_camera(const vec3f& offset, const Angles& angles)
    {
        scene.camera.add_relative_angles(angles);
        scene.camera.add_relative_offset(offset);
        scene.camera.update_matrices();
    }
    
    void OnResize(uint32_t width, uint32_t height) override
    {
	    scene.init_depth_and_stencil_buffer(width, height);
        scene.camera.change_aspect(float(width) / height);
    }
    void KeyEvent(Key key, bool status) override { is.keyboard.keys[key] = status; } 
    void MouseWheelEvent(uint32_t count) override { is.mouse.wheel += count; }
    void MouseEvent(Key button, BUTTON status, uint32_t x_pos, uint32_t y_pos) override;

    void process_input(float dt);
    void process_gui_input();

    void init_scene();
};
