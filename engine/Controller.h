#pragma once
#include <cstdint>

#include "Scene.h"
#include "math/math.h"

enum BUTTON { PRESSED, DOWN, RELEASED, UP };

struct Mouse
{
    int16_t lmb_x, lmb_y;
	int16_t prev_x, prev_y;
	int16_t x, y;

	BUTTON rmb{ UP };
	BUTTON lmb{ UP };

};

struct Keyboard
{
	bool forward, backward, left, right, up, down;
    bool yawleft, yawright, pitchup, pitchdown;
    bool lroll, rroll;
	bool exit;

	Keyboard() : forward(false), backward(false), left(false), right(false),
	up(false), down(false), lroll(false), rroll(false), exit(false)
	{
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
				rotation_speed = 1.f,
				rotation_speed_mouse = 2.f * PI;

    IntersectionQuery record;

    Scene& scene;
    Camera& camera;
public:

    explicit Controller(Scene& rscene, Camera& camera): scene(rscene), camera(camera)
    {
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
