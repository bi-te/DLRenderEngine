#include "Controller.h"
#include "Material.h"
void Controller::init_scene()
{
    //materials
    Material material;
    material.albedo = { 0.1f, 0.5f, 0.3f };
    material.emission = { 0.f, 0.f, 0.f };
    material.specular = 0.7f;
    material.glossiness = 64.f;
    scene.materials.push_back(material);// 0 - green sphere

    material.albedo = { 0.f, 0.f, 0.f };
    material.emission = { 255.f, 0.f, 0.f };
    material.specular = 0.f;
    material.glossiness = 0.f;
    scene.materials.push_back(material);// 1 - red light

    // spheres
    Sphere sphere{ {2, 2, 50}, 5 };
    SphereObject spo{ sphere, 0 };
    scene.spheres.push_back(spo);

    sphere = { {400, 250, 40}, 5 };
    spo = { sphere, 1 };
    //scene.spheres.push_back(spo);

    //direct light
    scene.sunlight.direction = normalize({ 0.f, -1.f, 0.25f });
    scene.sunlight.light = {100, 100, 100};

    //point lights
    PointLight pl{};
    pl.position = { 10, 10, 0 };
    pl.light = { 105, 5, 5 };
    pl.light_distance = 100;
    scene.point_lights.push_back(pl);

    //spot lights
    Spotlight sp{};
    sp.position = { 0, 0, 0 };
    sp.light = { 5, 5, 155 };
    sp.light_distance = 100;
    sp.direction = normalize({ 0.f, 0.f, 1.f });
    sp.cutOff = cosf(to_radians(12));
    sp.outerCutOff = cosf(to_radians(15));
    scene.spotlights.push_back(sp);

    //spot lights
}

void Controller::process_input(float dt)
{
    InputState& is = input_state();

    if (is.keyboard.exit) {
        PostQuitMessage(0);
        return;
    }

    float dist = 50 * dt;

    if (is.keyboard.up) move_scene(0, dist, 0);
    if (is.keyboard.left) move_scene(-dist, 0, 0);
    if (is.keyboard.down) move_scene(0, -dist, 0);
    if (is.keyboard.right) move_scene(dist, 0, 0);

    float speed = 3 * dt;
    switch (is.mouse.rmb)
    {
    case PRESSED:
        is.mouse.rmb = DOWN;
        break;
    case DOWN:
        move_scene(speed * (is.mouse.x - is.mouse.prev_x),
            speed * (is.mouse.prev_y - is.mouse.y), 0);

        break;
    case RELEASED:
        is.mouse.rmb = UP;
    case UP:
        break;
    }

    is.mouse.prev_x = is.mouse.x;
    is.mouse.prev_y = is.mouse.y;

}
