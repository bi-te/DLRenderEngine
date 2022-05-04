#include "Controller.h"

void Controller::process_input(float dt)
{
    InputState& is = input_state();

    if (is.keyboard.exit) {
        PostQuitMessage(0);
        return;
    }

    float dist = 500 * dt;

    if (is.keyboard.up) move_scene(0, -dist, 0);
    if (is.keyboard.left) move_scene(-dist, 0, 0);
    if (is.keyboard.down) move_scene(0, dist, 0);
    if (is.keyboard.right) move_scene(dist, 0, 0);


    switch (is.mouse.rmb)
    {
    case PRESSED:
        is.mouse.rmb = DOWN;
        break;
    case DOWN:
        move_scene(float(is.mouse.x - is.mouse.prev_x),
            float(is.mouse.y - is.mouse.prev_y), 0);

        break;
    case RELEASED:
        is.mouse.rmb = UP;
    case UP:
        break;
    }

    is.mouse.prev_x = is.mouse.x;
    is.mouse.prev_y = is.mouse.y;

}
