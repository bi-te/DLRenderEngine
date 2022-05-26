#include "Controller.h"

#include <iostream>

#include "Engine.h"
#include "render/Material.h"

void Controller::init_scene()
{
    //materials
    Material material;
    material.type = SURFACE;
    material.albedo = { 0.1f, 0.5f, 0.3f };
    material.emission = { 0.f, 0.f, 0.f };
    material.specular = 0.7f;
    material.glossiness = 64.f;
    scene.materials.push_back(material);// 0 - green 

    material.type = LIGHT_SOURCE;
    material.albedo = { 0.f, 0.f, 0.f };
    material.emission = { 255.f, 0.f, 0.f };
    material.specular = 0.f;
    material.glossiness = 0.f;
    scene.materials.push_back(material);// 1 - red light

    material.type = SURFACE;
    material.albedo = { 0.56f, 1.f, 0.38f };
    material.emission = { 0.f, 0.f, 0.f };
    material.specular = 0.15f;
    material.glossiness = 2.f;
    scene.materials.push_back(material);// 2 - light green

    material.type = SURFACE;
    material.albedo = { 0.45f, 0.25f, 0.45f };
    material.emission = { 0.f, 0.f, 0.f };
    material.specular = 0.5f;
    material.glossiness = 16.f;
    scene.materials.push_back(material);// 3 - dark purple

    material.type = LIGHT_SOURCE;
    material.albedo = { 0.f, 0.f, 0.f };
    material.emission = { 0.f, 0.f, 255.f };
    material.specular = 0.f;
    material.glossiness = 0.f;
    scene.materials.push_back(material);// 4 - blue light

    // spheres
    Sphere sphere{ {0.f, 3.f, 30.f}, 5.f };
    SphereObject spo{ sphere, 0 };
    scene.spheres.push_back(spo); // green sphere

    //direct light
    scene.sunlight.direction = vec3{ 0.f, -1.f, 0.25f }.normalized();
    scene.sunlight.light = {100.f, 100.f, 100.f }; // white direct light

    //point lights
    PointLightObject pl;
    pl.plight.position = { 10.f, 20.f, 0.f };
    pl.plight.light = { 105.f, 5.f, 5.f };
    pl.plight.light_range = 100.f;
    pl.sphere.center = pl.plight.position;
    pl.sphere.radius = 0.5f;
    pl.material = 1;
    scene.point_lights.push_back(pl); // red point light

    //spot lights
    SpotlightObject sp{};
    sp.spotlight.position = { 0.f, 0.f, 100.f };
    sp.spotlight.light = { 5.f, 5.f, 155.f };
    sp.spotlight.light_range = 100.f;
    sp.spotlight.direction = vec3{ 0.f, 0.f, -1.f }.normalized();
    sp.spotlight.cutOff = cosf(to_radians(7.f));
    sp.spotlight.outerCutOff = cosf(to_radians(12.f));
    sp.sphere.center = sp.spotlight.position;
    sp.sphere.radius = 0.5f;
    sp.material = 4;
    scene.spotlights.push_back(sp); // blue spotlight

    //floor
    scene.floor.plane = { vec3{0.f, 1.f, 0.f}, {0.f, -40.f, 0.f}};
    scene.floor.material = 2;

    //cubes
    MeshInstance instance;
    instance.mesh = &scene.cube;
    instance.transform.set_world_offset({10.f, -10.f, 5.f });
    instance.transform.set_scale({5.f, 5.f, 5.f });
    instance.transform.set_world_rotation({0.f, 0.f, 60.f });
    instance.transform.update();
    instance.material = 0;
    scene.meshes.push_back(instance);

    instance.mesh = &scene.cube;
    instance.transform.set_world_offset({ -10.f, 8.f, 0.f });
    instance.transform.set_scale({10.f, 10.f, 10.f });
    instance.transform.set_world_rotation({45.f, 0.f, 0.f });
    instance.transform.update();
    instance.material = 3;
    scene.meshes.push_back(instance);

    camera.set_world_offset({ 0.f, 0.f, -100.f });
}

void Controller::process_input(float dt)
{
    Screen& screen = Engine::instance().screen;
    InputState& is = input_state();

    if (is.keyboard.exit) {
        PostQuitMessage(0);
        return;
    }

    float dist = movement_speed * dt;

    vec3 move{0.f, 0.f, 0.f };
    Angles rot{};

    if (is.keyboard.forward) move.z() += dist;
    if (is.keyboard.backward) move.z() -= dist;
    if (is.keyboard.right) move.x() += dist;
    if (is.keyboard.left) move.x() -= dist;
    if (is.keyboard.up) move.y() += dist;
    if (is.keyboard.down) move.y() -= dist;

    float rspeed = rotation_speed * dt;

    if (is.keyboard.rroll) rot.roll += rspeed;
    if (is.keyboard.lroll) rot.roll -= rspeed;

    if (is.keyboard.yawleft) rot.yaw += rspeed;
    if (is.keyboard.yawright) rot.yaw -= rspeed;
    if (is.keyboard.pitchup) rot.pitch += rspeed;
    if (is.keyboard.pitchdown) rot.pitch -= rspeed;

    float mspeedx = rotation_speed_mouse * dt / screen.width();
    float mspeedy = rotation_speed_mouse * dt / screen.height();

    switch (is.mouse.lmb)
    {
    case PRESSED:
        is.mouse.lmb = DOWN;
        break;
    case DOWN:
        rot.yaw += float(is.mouse.lmb_x - is.mouse.x) * mspeedx;
        rot.pitch += float(is.mouse.lmb_y - is.mouse.y) * mspeedy;

        break;
    case RELEASED:
        is.mouse.lmb = UP;
    case UP:
        break;
    }

    bool camera_update = true;
    float dx, dy, h, w, prop;
    vec4 up, right;
    vec3 offset, view, tview, trans;
    Ray mouse_ray;
    quat rotation;

    switch (is.mouse.rmb)
    {
    case PRESSED:
        up = camera.tlnear_fpoint - camera.blnear_fpoint;
        right = camera.brnear_fpoint - camera.blnear_fpoint;

        dx = (is.mouse.x + 0.5f) / screen.width();
        dy = 1.f - (is.mouse.y + 0.5f)/ screen.height();

        mouse_ray.origin = camera.position();
        mouse_ray.direction = ((camera.blnear_fpoint + right * dx + up * dy).head<3>() - mouse_ray.origin).normalized();

        scene.select_object(mouse_ray, camera.zn, camera.zf, record);

        is.mouse.rmb = DOWN;

    case DOWN:
        if (record.mover.get())
        {
            h = 2.f * camera.zn / camera.proj(1, 1);
            w = camera.aspect * h;

            prop = fabsf((record.intersection.point * camera.view.col(2).head<3>() + camera.view(3, 2)) / camera.zn);

            rotation = quat{ Eigen::AngleAxisf{rot.roll, vec3{0.f, 0.f,1.f}} };
            rotation *= quat{ Eigen::AngleAxisf{rot.pitch, vec3{1.f, 0.f,0.f}} };
            rotation *= quat{ Eigen::AngleAxisf{rot.yaw, vec3{0.f, 1.f,0.f}} };
            
            view = record.intersection.point * camera.view.topLeftCorner<3, 3>() + camera.view.row(3).head<3>();            
            tview = view * rotation.toRotationMatrix();
            trans = tview - view;
            trans *= camera.view_inv.topLeftCorner<3, 3>();

            move_camera(move, rot);
            camera_update = false;

            offset = trans;
            offset += move.x() * camera.right() + move.y() * camera.up() + move.z() * camera.forward();
            offset += (is.mouse.x - is.mouse.prev_x) * w * prop / screen.width() * camera.right();
            offset += (is.mouse.prev_y - is.mouse.y) * h * prop / screen.height() * camera.up();

            ////FPS
            //rotation = quat{ Eigen::AngleAxisf{rot.pitch, vec3{1.f, 0.f,0.f}} };
            //rotation *= quat{ Eigen::AngleAxisf{
            //    rot.yaw,
            //    vec3{0,1,0} *camera.view.topLeftCorner<3, 3>()}};

            //view = record.intersection.point * camera.view.topLeftCorner<3, 3>() + camera.view.row(3).head<3>();
            //tview = view * rotation.toRotationMatrix();
            //trans = tview - view;
            //trans *= camera.view_inv.topLeftCorner<3, 3>();

            //move_camera(move, rot);
            //camera_update = false;

            //offset = trans;
            //offset += move.x() * camera.right() + move.y() * camera.up() + move.z() * camera.forward();
            //offset += (is.mouse.x - is.mouse.prev_x) * w * prop / screen.width() * camera.right();
            //offset += (is.mouse.prev_y - is.mouse.y) * h * prop / screen.height() * camera.up();

            record.intersection.point += offset;
            record.mover->move(offset);
        }

        break;
    case RELEASED:
        is.mouse.rmb = UP;
    case UP:
        break;
    }

    if(camera_update)
    {
        move_camera(move, rot);
    }

    is.mouse.prev_x = is.mouse.x;
    is.mouse.prev_y = is.mouse.y;
}
