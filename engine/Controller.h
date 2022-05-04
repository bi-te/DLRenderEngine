#pragma once
#include <cstdint>

#include "Scene.h"
#include "Window.h"
#include "math/Ray.h"
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
	bool up, down, left, right;
	bool exit;

	Keyboard() : up(false), down(false), left(false), right(false), exit(false)
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
public:

    explicit Controller(Scene& rscene): scene(rscene)
    {
    }

    static InputState& input_state()
    {
        static InputState is{};
        return is;
    }

    void move_scene(float dx, float dy, float dz)
    {
        for (Sphere& sphere : scene.objects)
        {
            sphere.center += Vec3{ dx, dy, dz };
        }
    }

    void process_input(float dt);

};
