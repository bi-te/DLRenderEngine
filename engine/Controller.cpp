#include "Controller.h"

#include <iostream>

#include "Engine.h"
#include "imgui/ImGuiManager.h"
#include "includes/win.h"
#include "math/CubeMesh.h"
#include "render/ShaderManager.h"
#include "render/TextureManager.h"

void Controller::init_scene()
{
    ShaderManager& shaders = ShaderManager::instance();
    shaders.add_shader(L"shaders/default.hlsl", "main", "ps_main");
    scene.skybox.shader = shaders.add_shader(L"shaders/sky.hlsl", "main", "ps_main");

    TextureManager& texture_manager = TextureManager::instance();
    texture_manager.add_texture(L"assets/textures/woodm.dds");
    scene.skybox.texture = texture_manager.add_texture(L"assets/cubemaps/skyboxbm.dds");


    std::vector<Mesh>& meshes = Engine::instance().scene.meshes;
    CubeMesh cube{};
    meshes.push_back(std::move(cube));

    std::vector<MeshInstance>& instances = Engine::instance().scene.instances;
    MeshInstance instance;
    instance.mesh = &meshes[0];
    instance.render_data.texture = 0;
    instance.render_data.shader = 0;
    instance.render_data.transformation = { sizeof(mat4) };
    instance.transform.set_world_offset({ 0.f, 0.f, 30.5f });
    instance.transform.set_scale({ 15.f, 15.f, 15.f });
    instance.transform.add_world_offset({0.f, 0.f, 5.f});
    instances.push_back(std::move(instance));

    scene.init_objects_buffers();
}

void Controller::process_gui_input()
{
    ImGui::Begin("Render Settings");

    static const char* filter_labels[] = {
        "MIN_MAG_MIP_POINT",
        "MIN_MAG_POINT_MIP_LINEAR",
        "MIN_POINT_MAG_LINEAR_MIP_POINT",
        "MIN_POINT_MAG_MIP_LINEAR",
        "MIN_LINEAR_MAG_MIP_POINT",
        "MIN_LINEAR_MAG_POINT_MIP_LINEAR",
        "MIN_MAG_LINEAR_MIP_POINT",
        "MIN_MAG_MIP_LINEAR",
        "ANISOTROPIC"
    };

    static D3D11_FILTER filters[] = {
        D3D11_FILTER_MIN_MAG_MIP_POINT,
        D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR,
        D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT,
        D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR ,
        D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT ,
        D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR ,
        D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT ,
        D3D11_FILTER_MIN_MAG_MIP_LINEAR ,
        D3D11_FILTER_ANISOTROPIC,
    };

    static int index = 7;
    if (ImGui::Combo("Filter", &index, filter_labels, ARRAYSIZE(filter_labels)))
    {
        renderer.init_sampler_state(filters[index]);
    }

    ImGui::End();
}

void Controller::process_input(float dt)
{
    InputState& is = input_state();
    ImageSettings& im = image_settings();
    float screen_width = Engine::instance().renderer.buffer_width(),
		  screen_height = Engine::instance().renderer.buffer_height();

    //image settings
    //if (is.keyboard.keys[PLUS])  im.ev100 += 0.1f;
    //if (is.keyboard.keys[MINUS]) im.ev100 -= 0.1f;
    //if (is.keyboard.keys[R]) {
    //    im.reflection = !im.reflection;
    //    is.keyboard.keys[R] = false;
    //}
    //if (is.keyboard.keys[P]) {
    //    im.progressive_gi = !im.progressive_gi;
    //    im.gi_frame = 0;
    //    is.keyboard.keys[P] = false;
    //}
    //if(is.keyboard.keys[G])
    //{
    //    im.global_illumination = GI_ON;
    //    is.keyboard.keys[G] = false;
    //}

    if (is.keyboard.keys[I]) {
        ImGuiManager::active() = !ImGuiManager::active();
        is.keyboard.keys[I] = false;
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

    float mspeedx = rotation_speed_mouse * dt / screen_width;
    float mspeedy = rotation_speed_mouse * dt / screen_height;
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

        dx = (is.mouse.x + 0.5f) / screen_width;
        dy = 1.f - (is.mouse.y + 0.5f) / screen_height;

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
                offset += (is.mouse.x - is.mouse.prev_x) * w * prop / screen_width * camera.right();
                offset += (is.mouse.prev_y - is.mouse.y) * h * prop / screen_height * camera.up();
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
                offset += (is.mouse.x - is.mouse.prev_x) * w * prop / screen_width * camera.right();
                offset += (is.mouse.prev_y - is.mouse.y) * h * prop / screen_height * camera.up();
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
