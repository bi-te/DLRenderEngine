#include "Controller.h"

#include <iostream>

#include "Engine.h"
#include "imgui/ImGuiManager.h"
#include "render/MaterialManager.h"
#include "render/MeshSystem.h"
#include "render/ModelManager.h"
#include "render/ShaderManager.h"
#include "render/TextureManager.h"

void Controller::init_scene()
{
    ShaderManager& shaders = ShaderManager::instance();
    shaders.add_shader(L"shaders/default.hlsl", "main", "ps_main");
    shaders.add_shader(L"shaders/sky.hlsl", "main", "ps_main");

    TextureManager& texture_manager = TextureManager::instance();
    texture_manager.add_texture(L"assets/textures/woodm.dds");
    texture_manager.add_texture(L"assets/cubemaps/skyboxbm.dds");

    ModelManager& models = ModelManager::instance();
    MaterialManager& materials = MaterialManager::instance();
    materials.add_material("Wood_mat", { {TextureDiffuse, L"assets/textures/woodm.dds"} });

    models.add_model("assets/models/Samurai/Samurai.fbx");
    models.add_model("assets/models/Knight/Knight.fbx");
    models.add_model("assets/models/KnightHorse/KnightHorse.fbx");
    models.add_model("assets/models/SunCityWall/SunCityWall.fbx");
    models.make_cube();

    models.add_model("assets/models/InstanceTest/test.fbx");
    Instance test;
    test.model_world.set_world_offset({ 20.f, 0.f, -20.f });
    test.model_world.set_scale(0.2f);
    test.model_world.set_world_rotation({0.f, 0.f, to_radians(90.f)});
    MeshSystem::instance().opaque_instances.add_model_instance(
        models.get_ptr("assets/models/InstanceTest/test.fbx"),
        {
        	materials.get("assets/models/InstanceTest/Cube_mat"),
            materials.get("assets/models/InstanceTest/Sphere_mat")
        },
        test
    );

    Instance cube;
    cube.model_world.set_scale({100.f, 10.f, 10.f});
    cube.model_world.set_world_offset({ 0.f, 0.f, 55.f });
    MeshSystem::instance().opaque_instances.add_model_instance(models.get_ptr("Cube"), { materials.get("Wood_mat") }, cube);

    cube.model_world.set_scale({ 10.f, 50.f, 10.f });
    cube.model_world.set_world_offset({ -76.f, 0.f, 0.f });
    MeshSystem::instance().opaque_instances.add_model_instance(models.get_ptr("Cube"), { materials.get("assets/models/SunCityWall/Wall_mat")}, cube);


    cube.model_world.set_scale(10.f);
    for (int row = 0; row < 4; ++row)
    {
	    for (int column = 0; column < 2; ++column)
	    {
            cube.model_world.set_world_offset({ column * 11.f, -10.f, row * -11.f });
            MeshSystem::instance().opaque_instances.add_model_instance(models.get_ptr("Cube"), { materials.get("assets/models/Knight/Skirt_mat") }, cube);
	    }
        
    }



	Instance samurai;
    samurai.model_world.set_scale(5.f);
    samurai.model_world.set_world_offset({ 10.f, 0.f, 0.f });
    MeshSystem::instance().opaque_instances.add_model_instance(
        models.get_ptr("assets/models/Samurai/Samurai.fbx"),
        {
            materials.get("assets/models/Samurai/Sword_mat"),
            materials.get("assets/models/Samurai/Head_mat"),
            materials.get("assets/models/Samurai/Eyes_mat"),
            materials.get("assets/models/Samurai/Helmet_mat"),
            materials.get("assets/models/Samurai/Skirt_mat"),
            materials.get("assets/models/Samurai/Legs_mat"),
            materials.get("assets/models/Samurai/Hands_mat"),
            materials.get("assets/models/Samurai/Torso_mat")
        },
        samurai
    );

    Instance knight;
    knight.model_world.set_scale(5.f);
    knight.model_world.set_world_offset({ 20.f, 0.f, 0.f });
    MeshSystem::instance().opaque_instances.add_model_instance(
        models.get_ptr("assets/models/Knight/Knight.fbx"),
        {
            materials.get("assets/models/Knight/Fur_mat"),
        	materials.get("assets/models/Knight/Legs_mat"),
        	materials.get("assets/models/Knight/Torso_mat"),
        	materials.get("assets/models/Knight/Head_mat"),
        	materials.get("assets/models/Knight/Eyes_mat"),
        	materials.get("assets/models/Knight/Helmet_mat"),
        	materials.get("assets/models/Knight/Skirt_mat"),
        	materials.get("assets/models/Knight/Cape_mat"),
        	materials.get("assets/models/Knight/Gloves_mat")
        },
        knight
    );

    Instance horse;
    horse.model_world.set_scale(5.f);
    horse.model_world.set_world_offset({ 30.f, 0.f, 0.f });
    MeshSystem::instance().opaque_instances.add_model_instance(
        models.get_ptr("assets/models/KnightHorse/KnightHorse.fbx"),
        {
            materials.get("assets/models/KnightHorse/Armor_mat"),
            materials.get("assets/models/KnightHorse/Horse_mat"),
            materials.get("assets/models/KnightHorse/Tail_mat")
        },
        horse
    );

    Instance wall;
    wall.model_world.set_scale(5.f);
    wall.model_world.set_world_offset({ 0.f, 0.f, 10.f });
    MeshSystem::instance().opaque_instances.add_model_instance(
        models.get_ptr("assets/models/SunCityWall/SunCityWall.fbx"),
        {
            materials.get("assets/models/SunCityWall/Star_mat"),
            materials.get("assets/models/SunCityWall/Wall_mat"),
            materials.get("assets/models/SunCityWall/Trims_mat"),
            materials.get("assets/models/SunCityWall/Statue_mat"),
            materials.get("assets/models/SunCityWall/Stonework_mat"),
        }, 
        wall
    );

    scene.skybox.shader = L"shaders/sky.hlsl";
    scene.skybox.texture = L"assets/cubemaps/skyboxbm.dds";



    //std::vector<Mesh>& meshes = Engine::instance().scene.meshes;
    //CubeMesh cube{};
    //meshes.push_back(std::move(cube));

    //std::vector<MeshInstance>& instances = Engine::instance().scene.instances;
    //MeshInstance instance;
    //instance.mesh = &meshes[0];
    //instance.render_data.texture = L"assets/textures/woodm.dds";
    //instance.render_data.shader = L"shaders/default.hlsl";
    //instance.render_data.transformation = { sizeof(mat4f), Direct3D::instance().device5};
    //instance.transform.set_world_offset({ 0.f, 0.f, 30.5f });
    //instance.transform.set_scale({ 15.f, 15.f, 15.f });
    //instance.transform.add_world_offset({0.f, 0.f, 5.f});
    //instances.push_back(std::move(instance));

    scene.init_depth_and_stencil_buffer(window.width(), window.height());
    scene.init_depth_stencil_state();
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

    static int index = 7, manisotropy = 0;
    if (ImGui::Combo("Filter", &index, filter_labels, ARRAYSIZE(filter_labels)))
    {
        Direct3D::instance().init_sampler_state(filters[index], manisotropy);
    }
    
    if (ImGui::SliderInt("Max Anisotropy", &manisotropy, D3D11_MIN_MAXANISOTROPY, D3D11_MAX_MAXANISOTROPY) 
        && filters[index] == D3D11_FILTER_ANISOTROPIC)
        Direct3D::instance().init_sampler_state(filters[index], manisotropy);

    ImGui::End();
}

void Controller::MouseEvent(Key button, BUTTON status, uint32_t x_pos, uint32_t y_pos)
{
    switch (button)
    {
    case LMOUSE:
        is.mouse.lmb = status;
        is.mouse.lmb_x = x_pos;
        is.mouse.lmb_y = y_pos;
        break;
    case RMOUSE:
        is.mouse.rmb = status;
        break;
    }
    is.mouse.x = x_pos;
    is.mouse.y = y_pos;
}

void Controller::process_input(float dt)
{
    float screen_width = window.width(),
		  screen_height = window.height();

    if (is.keyboard.keys[I]) {
        ImGuiManager::active() = !ImGuiManager::active();
        is.keyboard.keys[I] = false;
    }

    vec3f move{ 0.f, 0.f, 0.f };
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
    vec4f up, right;
    vec3f view, tview, trans, offset{ 0.f, 0.f, 0.f };
    Ray mouse_ray;
    quatf rotation;

    switch (is.mouse.rmb)
    {
    case PRESSED:
        up = scene.camera.frustrum_up;
        right = scene.camera.frustrum_right;

        dx = (is.mouse.x + 0.5f) / screen_width;
        dy = 1.f - (is.mouse.y + 0.5f) / screen_height;

        mouse_ray.origin = scene.camera.position();
        mouse_ray.direction = ((scene.camera.blnear_fpoint + right * dx + up * dy).head<3>() - mouse_ray.origin).normalized();

        scene.select_object(mouse_ray, scene.camera.zn, scene.camera.zf, record);

        is.mouse.rmb = MDOWN;

    case MDOWN:
        if (record.mover.get())
        {
            h = 2.f * scene.camera.zn / scene.camera.proj(1, 1);
            w = scene.camera.aspect * h;

            prop = fabsf((record.intersection.point * scene.camera.view.col(2).head<3>() + scene.camera.view(3, 2)) / scene.camera.zn);

            if(scene.camera.fps_camera)
            {
                //FPS
                rotation = quatf{ Eigen::AngleAxisf{rot.pitch, vec3f{1.f, 0.f,0.f}} };
                rotation *= quatf{ Eigen::AngleAxisf{
                    rot.yaw,
                    vec3f{0,1,0} *scene.camera.view.topLeftCorner<3, 3>()} };

                view = record.intersection.point * scene.camera.view.topLeftCorner<3, 3>() + scene.camera.view.row(3).head<3>();
                tview = view * rotation.toRotationMatrix();
                trans = tview - view;
                trans *= scene.camera.view_inv.topLeftCorner<3, 3>();

                move_camera(move, rot);
                camera_update = false;

                offset = trans;
                offset += move.x() * scene.camera.right() + move.y() * scene.camera.up() + move.z() * scene.camera.forward();
                offset += (is.mouse.x - is.mouse.prev_x) * w * prop / screen_width * scene.camera.right();
                offset += (is.mouse.prev_y - is.mouse.y) * h * prop / screen_height * scene.camera.up();
            } else
            {
                //spaceship
                rotation = quatf{ Eigen::AngleAxisf{rot.roll, vec3f{0.f, 0.f,1.f}} };
                rotation *= quatf{ Eigen::AngleAxisf{rot.pitch, vec3f{1.f, 0.f,0.f}} };
                rotation *= quatf{ Eigen::AngleAxisf{rot.yaw, vec3f{0.f, 1.f,0.f}} };

                view = record.intersection.point * scene.camera.view.topLeftCorner<3, 3>() + scene.camera.view.row(3).head<3>();
                tview = view * rotation.toRotationMatrix();
                trans = tview - view;
                trans *= scene.camera.view_inv.topLeftCorner<3, 3>();

                move_camera(move, rot);
                camera_update = false;

                offset = trans;
                offset += move.x() * scene.camera.right() + move.y() * scene.camera.up() + move.z() * scene.camera.forward();
                offset += (is.mouse.x - is.mouse.prev_x) * w * prop / screen_width * scene.camera.right();
                offset += (is.mouse.prev_y - is.mouse.y) * h * prop / screen_height * scene.camera.up();
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

    //if( move.x() != 0.f || move.y() != 0.f  || move.z() != 0.f ||
    //    rot.yaw != 0.f  || rot.pitch != 0.f || rot.roll != 0.f ||
    //    offset.x() != 0.f || offset.y() != 0.f || offset.z() != 0.f )
    //    im.gi_frame = 0;

    is.mouse.prev_x = is.mouse.x;
    is.mouse.prev_y = is.mouse.y;
}
