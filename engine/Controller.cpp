#include "Controller.h"

#include <iostream>

#include "Engine.h"

void Controller::init_scene()
{
    //materials
    Material material;
    material.type = SURFACE;
    material.albedo = { 0.45f, 0.25f, 0.45f };
    material.emission = { 0.f, 0.f, 0.f };
    material.f0 = {0.04f, 0.04f, 0.04f};
    material.roughness = 0.7f;
    material.metalness = 1.f;
    scene.materials.push_back(material);// 0 - dark purple 

    material.type = SURFACE;
    material.albedo = { 0.36f, 0.85f, 0.24f };
    material.emission = { 0.f, 0.f, 0.f };
    material.f0 = { 0.03f, 0.03f, 0.03f };
    material.roughness = 0.9f;
    material.metalness = 0.f;
    scene.materials.push_back(material);// 1 - light green

    material.type = SURFACE;
    material.emission = { 0.f, 0.f, 0.f };
    material.albedo = { 0.67f, 0.67f, 0.15f };
    material.f0 = { 0.05f, 0.05f, 0.05f };
    material.roughness = 0.05f;
    material.metalness = 0.f;
    scene.materials.push_back(material);// 2

    material.type = LIGHT_SOURCE;
    material.albedo = { 0.f, 0.f, 0.f };
    material.emission = { 40000.f, 40000.f, 40000.f };
    material.f0 = { 0.f, 0.f, 0.f };
    material.roughness = 0.f;
    material.metalness = 0.f;
    scene.materials.push_back(material);// 3 - white light

    // spheres
    Sphere sphere;
    SphereObject spo;

    int8_t mx = 6, my = 6;
    float drough = (1.f - 0.01f) / (mx - 1);
    float dmetal = 1.f / (my - 1);
    float radius = 5.f;
    for (uint8_t x = 0; x < mx; ++x)
    {
	    for (uint8_t z = 0; z < my; ++z)
	    {
            material.type = SURFACE;
            material.emission = { 0.f, 0.f, 0.f };

            material.roughness = powf(0.01f + x * drough, 2.f);
            material.metalness = z * dmetal;
            material.albedo = vec3{ 0.8f, 0.f, 0.f };
            material.f0 = lerp(vec3{ 0.03f, 0.03f, 0.03f }, material.albedo, material.metalness);
            scene.materials.push_back(material); //y + my * x + 4 

            sphere = { {2.5f * radius * x, 5.5f, 2.5f * radius * z}, radius };
            spo = { sphere, uint32_t(z + my * x + 4)};
            scene.spheres.push_back(spo);
	    }

    }

    //direct light
    scene.dirlight.direction = vec3{ 0.f, -1.f, 0.25f }.normalized();
    scene.dirlight.light = {100000.f, 100000.f, 100000.f }; // white direct light
    scene.dirlight.solid_angle = 6.418e-5f;

    //point lights
    PointLightObject pl;
    pl.plight.position = { 62.5f, 10.f, -7.f };
    pl.plight.light = { 40000.f, 40000.f, 40000.f };
    pl.sphere.center = pl.plight.position;
    pl.sphere.radius = 1.f;
    pl.material = 3;
    scene.point_lights.push_back(pl); // point light

	//cubes
    MeshInstance instance;
    instance.mesh = &scene.cube;
    instance.transform.set_world_offset({ 10.f, 0.f, -10.f });
    instance.transform.set_scale({ 5.f, 5.f, 5.f });
    instance.transform.set_world_rotation({ 0.f, 0.f, 60.f });
    instance.transform.update();
    instance.material = 2;
    scene.meshes.push_back(instance);

    instance.mesh = &scene.cube;
    instance.transform.set_world_offset({ -10.f, 15.f, 0.f });
    instance.transform.set_scale({ 10.f, 10.f, 10.f });
    instance.transform.set_world_rotation({ 45.f, 0.f, 0.f });
    instance.transform.update();
    instance.material = 0;
    scene.meshes.push_back(instance);

    //floor
    scene.floor.plane = { vec3{0.f, 1.f, 0.f}, {0.f, -20.f, 0.f}};
    scene.floor.material = 1;

    camera.change_znear(0.1f);
    camera.set_world_angles({ 0.f, to_radians(-15.f), 0.f });
    camera.set_world_offset({ 62.5f, 0.f, -10.f });

    image_settings().ev100 = 2.f;
    image_settings().set_gi_tests(5000);
}

void Controller::process_input(float dt)
{
    Screen& screen = Engine::instance().screen;
    InputState& is = input_state();
    ImageSettings& im = image_settings();

    if (is.keyboard.keys[ESCAPE]) {
        PostQuitMessage(0);
        return;
    }

    //image settings
    if (is.keyboard.keys[PLUS])  im.ev100 += 0.1f;
    if (is.keyboard.keys[MINUS]) im.ev100 -= 0.1f;
    if (is.keyboard.keys[R]) {
        im.reflection = !im.reflection;
        is.keyboard.keys[R] = false;
    }
    if (is.keyboard.keys[P]) {
        im.progressive_gi = !im.progressive_gi;
        im.gi_frame = 0;
        is.keyboard.keys[P] = false;
    }
    if(is.keyboard.keys[G])
    {
        im.global_illumination = GI_ON;
        is.keyboard.keys[G] = false;
    }


    vec3 move{ 0.f, 0.f, 0.f };
    Angles rot{};

    // movement
    float dist = movement_speed * dt;
    if (is.mouse.wheel) dist = is.mouse.wheel > 0 ? dist * is.mouse.wheel * dspeed : -dist / (is.mouse.wheel * dspeed);
    if (is.keyboard.keys[SHIFT]) dist *= shift;

    if (is.keyboard.keys[W]) move.z() += dist;
    if (is.keyboard.keys[S]) move.z() -= dist;
    if (is.keyboard.keys[D]) move.x() += dist;
    if (is.keyboard.keys[A]) move.x() -= dist;
    if (is.keyboard.keys[SPACE] || is.keyboard.keys[E]) move.y() += dist;
    if (is.keyboard.keys[C] || is.keyboard.keys[CTRL] ||is.keyboard.keys[Q]) move.y() -= dist;

    float rspeed = rotation_speed * dt;
    if (is.keyboard.keys[LEFT]) rot.yaw += rspeed;
    if (is.keyboard.keys[RIGHT]) rot.yaw -= rspeed;
    if (is.keyboard.keys[UP]) rot.pitch += rspeed;
    if (is.keyboard.keys[DOWN]) rot.pitch -= rspeed;

    float mspeedx = rotation_speed_mouse * dt / screen.width();
    float mspeedy = rotation_speed_mouse * dt / screen.height();
    switch (is.mouse.lmb)
    {
    case PRESSED:
        is.mouse.lmb = MDOWN;

    case MDOWN:
        rot.yaw += float(is.mouse.lmb_x - is.mouse.x) * mspeedx;
        rot.pitch += float(is.mouse.lmb_y - is.mouse.y) * mspeedy;

        break;
    case RELEASED:
        is.mouse.lmb = MUP;
    case MUP:
        break;
    }

    bool camera_update = true;
    float dx, dy, h, w, prop;
    vec4 up, right;
    vec3 view, tview, trans, offset{ 0.f, 0.f, 0.f };
    Ray mouse_ray;
    quat rotation;

    switch (is.mouse.rmb)
    {
    case PRESSED:
        up = camera.frustrum_up;
        right = camera.frustrum_right;

        dx = (is.mouse.x + 0.5f) / screen.width();
        dy = 1.f - (is.mouse.y + 0.5f)/ screen.height();

        mouse_ray.origin = camera.position();
        mouse_ray.direction = ((camera.blnear_fpoint + right * dx + up * dy).head<3>() - mouse_ray.origin).normalized();

        scene.select_object(mouse_ray, camera.zn, camera.zf, record);

        is.mouse.rmb = MDOWN;

    case MDOWN:
        if (record.mover.get())
        {
            h = 2.f * camera.zn / camera.proj(1, 1);
            w = camera.aspect * h;

            prop = fabsf((record.intersection.point * camera.view.col(2).head<3>() + camera.view(3, 2)) / camera.zn);

            if(camera.fps_camera)
            {
                //FPS
                rotation = quat{ Eigen::AngleAxisf{rot.pitch, vec3{1.f, 0.f,0.f}} };
                rotation *= quat{ Eigen::AngleAxisf{
                    rot.yaw,
                    vec3{0,1,0} *camera.view.topLeftCorner<3, 3>()} };

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
            } else
            {
                //spaceship
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
            }


            record.intersection.point += offset;
            record.mover->move(offset);
        }

        break;
    case RELEASED:
        is.mouse.rmb = MUP;
    case MUP:
        break;
    }

    if(camera_update)
    {
        move_camera(move, rot);
    }

    if( move.x() != 0.f || move.y() != 0.f  || move.z() != 0.f ||
        rot.yaw != 0.f  || rot.pitch != 0.f || rot.roll != 0.f ||
        offset.x() != 0.f || offset.y() != 0.f || offset.z() != 0.f )
        im.gi_frame = 0;

    is.mouse.prev_x = is.mouse.x;
    is.mouse.prev_y = is.mouse.y;
}
