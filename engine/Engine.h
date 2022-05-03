#pragma once

#include "InputState.h"
#include "Screen.h"
#include "RenderList.h"

class Engine
{
public:	
	RenderList world;
	Screen screen;
	InputState input_state;

	static Engine& instance()
	{
		static std::unique_ptr<Engine> engine_(new Engine);
		return  *engine_;
	}

    void process_input(double dt)
    {
        screen.resize();

        if (input_state.keyboard.exit) {
            PostQuitMessage(0);
            return;
        }

        double dist = 500 * dt;

        if (input_state.keyboard.up) world.move(0, -dist, 0);
        if (input_state.keyboard.left) world.move(-dist, 0, 0);
        if (input_state.keyboard.down) world.move(0, dist, 0);
        if (input_state.keyboard.right) world.move(dist, 0, 0);

        static std::shared_ptr<RenderObject> movable;
        switch (input_state.mouse.rmb)
        {
        case PRESSED:
            movable = world.choose_object(Ray{ {double(input_state.mouse.x), double(input_state.mouse.y), 0},
                {0, 0, 1} }, 0.1);
            input_state.mouse.rmb = DOWN;
            break;
        case DOWN:
            if (movable.get())
            {
                movable->move(double(input_state.mouse.x - input_state.mouse.prev_x), 
                    double(input_state.mouse.y - input_state.mouse.prev_y), 0);
            }
            break;
        case RELEASED:
            movable = nullptr;
            input_state.mouse.rmb = UP;
        case UP:
            break;
        }

        input_state.mouse.prev_x = input_state.mouse.x;
        input_state.mouse.prev_y = input_state.mouse.y;

    }
private:
	Engine() = default;
};