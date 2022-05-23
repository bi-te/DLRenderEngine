#include "Controller.h"

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
    material.albedo = { 0.3f, 0.3f, 0.7f };
    material.emission = { 0.f, 0.f, 0.f };
    material.specular = 0.5f;
    material.glossiness = 16.f;
    scene.materials.push_back(material);// 2 - bluish

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
    Sphere sphere{ {0, 3, 30}, 5.f };
    SphereObject spo{ sphere, 0 };
    scene.spheres.push_back(spo); // green sphere

    //direct light
    scene.sunlight.direction = vec3{ 0.f, -1.f, 0.25f }.normalized();
    scene.sunlight.light = {100, 100, 100}; // white direct light

    //point lights
    PointLightObject pl;
    pl.plight.position = { 10, 20, 0 };
    pl.plight.light = { 105, 5, 5 };
    pl.plight.light_range = 100;
    pl.sphere.center = pl.plight.position;
    pl.sphere.radius = 0.5f;
    pl.material = 1;
    scene.point_lights.push_back(pl); // red point light

    //spot lights
    SpotlightObject sp{};
    sp.spotlight.position = { 0, 0, 100 };
    sp.spotlight.light = { 5, 5, 155 };
    sp.spotlight.light_range = 100;
    sp.spotlight.direction = vec3{ 0.f, 0.f, -1.f }.normalized();
    sp.spotlight.cutOff = cosf(to_radians(7));
    sp.spotlight.outerCutOff = cosf(to_radians(12));
    sp.sphere.center = sp.spotlight.position;
    sp.sphere.radius = 0.5f;
    sp.material = 4;
    scene.spotlights.push_back(sp); // blue spotlight

    //floor
    scene.floor.plane = { vec3{0, 1, 0}, {0, -40, 0}};
    scene.floor.material = 2;

    //cubes
    MeshInstance instance;
    instance.mesh = &scene.cube;
    instance.transform.set_world_offset({10, -10, 5});
    instance.transform.set_scale({5, 5, 5});
    instance.transform.set_world_rotation({0, 0, 60});
    instance.transform.update();
    instance.material = 0;
    scene.meshes.push_back(instance);

    instance.mesh = &scene.cube;
    instance.transform.set_world_offset({ -10, 8, 0 });
    instance.transform.set_scale({10, 10, 10});
    instance.transform.set_world_rotation({45, 0, 0});
    instance.transform.update();
    instance.material = 3;
    scene.meshes.push_back(instance);

    camera.set_world_offset({ 0, 0, -100 });
}

void Controller::process_input(float dt)
{
    Screen& screen = Engine::instance().screen;
    InputState& is = input_state();

    if (is.keyboard.exit) {
        PostQuitMessage(0);
        return;
    }

    float dist = 50 * dt;

    vec3 move{0, 0, 0};
    Angles rot{};

    if (is.keyboard.forward) move.z() += dist;
    if (is.keyboard.backward) move.z() -= dist;
    if (is.keyboard.right) move.x() += dist;
    if (is.keyboard.left) move.x() -= dist;
    if (is.keyboard.up) move.y() += dist;
    if (is.keyboard.down) move.y() -= dist;

    float dspeed = 1.f * dt;
    if (is.keyboard.rroll) rot.roll += dspeed;
    if (is.keyboard.lroll) rot.roll -= dspeed;

    if (is.keyboard.yawleft) rot.yaw += dspeed;
    if (is.keyboard.yawright) rot.yaw -= dspeed;
    if (is.keyboard.pitchup) rot.pitch += dspeed;
    if (is.keyboard.pitchdown) rot.pitch -= dspeed;

    switch (is.mouse.lmb)
    {
    case PRESSED:
        is.mouse.lmb = DOWN;
        break;
    case DOWN:
        rot.yaw += 0.1f * dspeed * (is.mouse.lmb_x - is.mouse.x) / 2.f;
        rot.pitch += 0.1f * dspeed * (is.mouse.lmb_y - is.mouse.y) / 2.f;

        break;
    case RELEASED:
        is.mouse.lmb = UP;
    case UP:
        break;
    }
    
    static IntersectionQuery record;

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

        dx = float(is.mouse.x) / screen.width();
        dy = 1.f - float(is.mouse.y)/ screen.height();

        mouse_ray.origin = camera.position();
        mouse_ray.direction = ((camera.blnear_fpoint + right * dx + up * dy).head<3>() - mouse_ray.origin).normalized();

        scene.select_object(mouse_ray, camera.zn, camera.zf, record);

        is.mouse.rmb = DOWN;

    case DOWN:
        if (record.mover.get())
        {
            h = 2 * camera.zn / camera.proj(1, 1);
            w = camera.aspect * h;

            prop = fabs((record.intersection.point * camera.view.col(2).head<3>() + camera.view(3, 2)) / camera.zn);

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
