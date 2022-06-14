#pragma once
#include <cstdint>

#include "Scene.h"
#include "math/math.h"

enum BUTTON { PRESSED, MDOWN, RELEASED, MUP };

struct Mouse
{
    int16_t lmb_x, lmb_y;
	int16_t prev_x, prev_y;
	int16_t x, y;
    int16_t wheel;

	BUTTON rmb{ MUP };
	BUTTON lmb{ MUP };

};

enum Key
{
    SHIFT = 0x10, CTRL = 0x11, ESCAPE = 0x1B, SPACE = 0x20, LEFT = 0x25, UP = 0x26, RIGHT = 0x27, DOWN = 0x28,
    A = 0x41, C = 0x43, D = 0x44, E = 0x45, G = 0x47, Q = 0x51, R = 0x52, S = 0x53, W = 0x57,
    PLUS = 0xBB, MINUS = 0xBD
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

class  Controller
{
    const float movement_speed = 50.f,
				shift = 5.f, dspeed = 1.1f,
				rotation_speed = 1.f,
				rotation_speed_mouse = 2.f * PI;

    IntersectionQuery record;

    Scene& scene;
    Camera& camera;
public:

    explicit Controller(Scene& rscene, Camera& camera): scene(rscene), camera(camera)
    {
    }
    
    static ImageSettings& image_settings()
    {
        static ImageSettings is{};
        return is;
    }

    static InputState& input_state()
    {
        static InputState is{};
        return is;
    }



    void move_camera(const vec3& offset, const Angles& angles)
    {
        camera.add_relative_angles(angles);
        camera.add_relative_offset(offset);
        camera.update_matrices();
    }

    void process_input(float dt);

    void init_scene();

};
