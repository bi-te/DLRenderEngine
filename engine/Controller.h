#pragma once
#include <cstdint>

#include "Scene.h"
#include "Window.h"
#include "math/math.h"
#include "math/Sphere.h"

enum BUTTON { PRESSED, DOWN, RELEASED, UP };

struct Mouse
{
	int16_t prev_x, prev_y;
	int16_t x, y;

	BUTTON rmb{ UP };
	BUTTON lmb{ UP };

};

struct Keyboard
{
	bool forward, backward, left, right, up, down;
    bool lroll, rroll;
    bool ar_up, ar_down, ar_left, ar_right;
	bool exit;

	Keyboard() : forward(false), backward(false), left(false), right(false),
	up(false), down(false), lroll(false), rroll(false), exit(false)
    ,ar_up(false), ar_down(false), ar_left(false), ar_right(false)
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

    void move_scene(float dx, float dy, float dz)
    {
        for (SphereObject& object : scene.spheres)
        {
            object.sphere.center += Vec3{ dx, dy, dz };
        }
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
