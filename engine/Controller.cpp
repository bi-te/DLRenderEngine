#include "Controller.h"

#include "Engine.h"
#include "EngineClock.h"
#include "imgui/ImGuiManager.h"
#include "moving/TransformSystem.h"
#include "render/LightSystem.h"
#include "render/MaterialManager.h"
#include "render/MeshSystem.h"
#include "render/ModelManager.h"
#include "render/ShaderManager.h"
#include "render/TextureManager.h"
#include "render/ParticleSystem.h"

void Controller::render()
{
    window.clear_buffer();
    scene.render(window.buffer, postProcess, camera);
    window.swap_buffer();
}

void init_floor(uint32_t floor_width, uint32_t floor_height)
{
    MeshSystem& meshes = MeshSystem::instance();
    ModelManager& models = ModelManager::instance();
    MaterialManager& materials = MaterialManager::instance();
    TransformSystem& transforms = TransformSystem::instance();

    float wmiddle = floor_width  / 2.f,
		  hmiddle = floor_height / 2.f;
    float scale = 10.f;
    Transform floor;
    floor.set_scale(scale);
    for (uint32_t column = 0; column < floor_width; ++column)
    {
        for (uint32_t row = 0; row < floor_height; ++row)
        {
            floor.set_world_offset({scale * (column - wmiddle) , 0.f, scale * (row - hmiddle)});
            meshes.opaque_instances.add_model_instance(
                models.get_ptr("Quad"),
                {
                    materials.get_opaque("Cobblestone_mat")
                },
                { transforms.transforms.insert(floor) }
            );
        }
    }
}

void Controller::init_scene()
{
    Direct3D& direct = Direct3D::instance();
    MeshSystem& meshes = MeshSystem::instance();
    LightSystem& lights = LightSystem::instance();
    ModelManager& models = ModelManager::instance();
    ShaderManager& shaders = ShaderManager::instance();
    MaterialManager& materials = MaterialManager::instance();
    TransformSystem& transforms = TransformSystem::instance();
    TextureManager& texture_manager = TextureManager::instance();
    ParticleSystem& particle_system = ParticleSystem::instance();

    shaders.add_shader(L"shaders/opaque.hlsl", "main", "ps_main");
    shaders.add_shader(L"shaders/emissive.hlsl", "main", "ps_main");
    shaders.add_shader(L"shaders/sky.hlsl", "main", "ps_main");
    shaders.add_shader(L"shaders/resolve.hlsl", "main", "ps_main");
    shaders.add_shader(L"shaders/resolve_ms.hlsl", "main", "ps_main");
    shaders.add_shader(L"shaders/omnidirectional_shadows.hlsl", "main", "gs_main", nullptr);
	shaders.add_shader(L"shaders/omnidirectional_grass_shadows.hlsl", "main", "gs_main", "ps_main");
    shaders.add_shader(L"shaders/directional_shadows.hlsl", "main", nullptr, nullptr);
    shaders.add_shader(L"shaders/particle.hlsl", "main", "ps_main");
    shaders.add_shader(L"shaders/grass.hlsl", "main", "ps_main");

    scene.skybox.skyshader = shaders.get_ptr(L"shaders/sky.hlsl");
    scene.grassfield.grassShader = shaders.get_ptr(L"shaders/grass.hlsl");
    scene.grassfield.pointShadowShader = shaders.get_ptr(L"shaders/omnidirectional_grass_shadows.hlsl");
    meshes.opaque_instances.opaqueShader = shaders.get_ptr(L"shaders/opaque.hlsl");
	meshes.opaque_instances.pointShadowShader = shaders.get_ptr(L"shaders/omnidirectional_shadows.hlsl");
    meshes.opaque_instances.spotShadowShader = shaders.get_ptr(L"shaders/directional_shadows.hlsl");
    meshes.emissive_instances.emissiveShader = shaders.get_ptr(L"shaders/emissive.hlsl");
    meshes.appearing_instances.appearShader = shaders.get_ptr(L"shaders/appearance.hlsl");
    particle_system.particle_shader = shaders.get_ptr(L"shaders/particle.hlsl");
    direct.depth_resolve_shader = shaders.get_ptr(L"shaders/depth_resolve.hlsl");

    scene.skybox.texture = texture_manager.add_cubemap(L"assets/cubemaps/night_street/night_street.dds");
    scene.skybox.irradiance_map = texture_manager.add_cubemap(L"assets/cubemaps/night_street/night_street_irradiance.dds");
    scene.skybox.load_reflection(L"assets/cubemaps/night_street/night_street_reflection.dds");
	scene.grassfield.baseColor = texture_manager.get(L"assets/textures/Grass/Grass_BaseColor.dds");
    scene.grassfield.normal = texture_manager.get(L"assets/textures/Grass/Grass_Normal.dds");
    scene.grassfield.roughness = texture_manager.get(L"assets/textures/Grass/Grass_Roughness.dds");
	scene.grassfield.opacity = texture_manager.get(L"assets/textures/Grass/Grass_Opacity2.dds");
    scene.grassfield.translucency = texture_manager.get(L"assets/textures/Grass/Grass_Translucency.dds");
    scene.grassfield.ambient_occlusion = texture_manager.get(L"assets/textures/Grass/Grass_AO.dds");
    direct.reflectance_map = texture_manager.add_cubemap(L"assets/cubemaps/reflectance.dds");
    meshes.appearing_instances.noiseTexture = texture_manager.add_texture(L"assets/cloud_noise.dds");

    particle_system.atlas.rows = 8u;
    particle_system.atlas.columns = 8u;
	particle_system.atlas.smoke_emva1 = texture_manager.add_texture(L"assets/particles/smoke/smoke_EMVA_1.dds");
    particle_system.atlas.smoke_emission = texture_manager.add_texture(L"assets/particles/smoke/smoke_emission_scatter.dds");
    particle_system.atlas.smoke_lightmap1 = texture_manager.add_texture(L"assets/particles/smoke/smoke_RLT_1.dds");
    particle_system.atlas.smoke_lightmap2 = texture_manager.add_texture(L"assets/particles/smoke/smoke_BotBF_1.dds");

    OpaqueMaterial material;
    
    material = {};
    material.name = "Crystal_mat";
    material.diffuse = texture_manager.add_texture(L"assets/textures/Crystal/Crystal_BaseColor.dds");
    material.normals = texture_manager.add_texture(L"assets/textures/Crystal/Crystal_Normal.dds");
	material.render_data.textures = MATERIAL_TEXTURE_DIFFUSE | MATERIAL_TEXTURE_NORMAL | MATERIAL_REVERSED_NORMAL_Y;
    material.render_data.metallic = 0.f;
    material.render_data.roughness = 0.1f;
    materials.add(material);

    material = {};
    material.name = "Cobblestone_mat";
    material.diffuse = texture_manager.add_texture(L"assets/textures/Cobblestone/Cobblestone_BaseColor.dds");
    material.normals = texture_manager.add_texture(L"assets/textures/Cobblestone/Cobblestone_Normal.dds");
    material.roughness = texture_manager.add_texture(L"assets/textures/Cobblestone/Cobblestone_Roughness.dds");
    material.render_data.textures = MATERIAL_TEXTURE_DIFFUSE | MATERIAL_TEXTURE_NORMAL | MATERIAL_TEXTURE_ROUGHNESS;
    material.render_data.metallic = 0.f;
    materials.add(material);

    material = {};
    material.name = "Stone_mat";
    material.diffuse = texture_manager.add_texture(L"assets/textures/Stone/Stone_BaseColor.dds");
    material.normals = texture_manager.add_texture(L"assets/textures/Stone/Stone_Normal.dds");
    material.roughness = texture_manager.add_texture(L"assets/textures/Stone/Stone_Roughness.dds");
    material.render_data.textures = MATERIAL_TEXTURE_DIFFUSE | MATERIAL_TEXTURE_NORMAL | MATERIAL_TEXTURE_ROUGHNESS | MATERIAL_REVERSED_NORMAL_Y;
    material.render_data.metallic = 0.f;
    materials.add(material);

    material = {};
    material.name = "CastleBrick_mat";
    material.diffuse = texture_manager.add_texture(L"assets/textures/CastleBrick/CastleBrick_BaseColor.dds");
    material.normals = texture_manager.add_texture(L"assets/textures/CastleBrick/CastleBrick_Normal.dds");
    material.roughness = texture_manager.add_texture(L"assets/textures/CastleBrick/CastleBrick_Roughness.dds");
    material.render_data.textures = MATERIAL_TEXTURE_DIFFUSE | MATERIAL_TEXTURE_NORMAL | MATERIAL_TEXTURE_ROUGHNESS;
    material.render_data.metallic = 0.f;
    materials.add(material);

    material = {};
    material.name = "Test_mat";
    material.render_data.roughness = 0.01f;
    material.render_data.metallic = 0.9f;
    material.render_data.diffuse = { 0.8f, 0.3f, 0.3f };
    materials.add(material);

    models.add_model("assets/models/Samurai/Samurai.fbx");
    //models.add_model("assets/models/Knight/Knight.fbx");
    //models.add_model("assets/models/KnightHorse/KnightHorse.fbx");
    models.add_model("assets/models/SunCityWall/SunCityWall.fbx");
    models.init_cube();
	models.init_quad();
    models.init_sphere(30, 20);
    models.init_flat_sphere(30, 20);
    models.init_flat_cube_sphere(20);
    
    //lights.set_ambient({ 0.1f, 0.1f, 0.1f });
    //lights.set_direct_light({ {5.f, 5.f, 5.f}, 0.1f, vec3f{0.f, -2.f, 1.f}.normalized() });

    Transform the_sun;
    the_sun.set_scale(0.5f);
    the_sun.set_world_offset({ -5.f, 1.f, -6.f });
    PointLight minisun = {
        {0.5f, 0.5f, 0.5f}, transforms.transforms.insert(the_sun), 0.5f, 5.f
    };
    lights.add_point_light(minisun, "Sphere");

    Transform pbr_trans;
    pbr_trans.set_world_offset({ 0.f, 0.f, -20.f });
    meshes.opaque_instances.add_model_instance(
          models.get_ptr("Sphere"),
        { materials.get_opaque("Test_mat") },
        { transforms.transforms.insert(pbr_trans) }
    );

    the_sun.set_world_offset({ 0.f, 20.f, -5.f });
    PointLight greensun = {
    {0.2f, 0.75f, 0.4f}, transforms.transforms.insert(the_sun), 0.5f, 3.f
    };
    lights.add_point_light(greensun, "Sphere");

    //Transform flashlight;
    //flashlight.set_scale(0.5f);
    //flashlight.set_world_offset({ 0.f, 5.f, -5.f });
    //Spotlight flash = {
    //    {0.5f, 0.5f, 0.5f}, transforms.transforms.insert(flashlight),
    //	{0.f, 0.f, 1.f}, 0.5f, rad(12.f), rad(17.f), 10.f
    //};
    //lights.add_spotlight(flash, "Sphere");

    //flashlight.set_scale(0.5f);
    //flashlight.set_world_offset({ -5.f, 5.f, -5.f });
    //Spotlight flash2 = {
    //    {0.3f, 0.1f, 0.5f}, transforms.transforms.insert(flashlight),
    //    {0.f, 0.f, 1.f}, 0.5f, rad(12.f), rad(17.f), 10.f
    //};
    //lights.add_spotlight(flash2, "Sphere");

    Transform sphere;
    sphere.set_world_offset({ -5.f, 20.f, 5.f });
    meshes.opaque_instances.add_model_instance(
        models.get_ptr("Sphere"),
        {
            materials.get_opaque("CastleBrick_mat")
        },
        { transforms.transforms.insert(sphere) }
    );

	Transform crystal_ball;
    crystal_ball.set_world_offset({ 5.f, 20.f, 5.f });
    meshes.opaque_instances.add_model_instance(
        models.get_ptr("Sphere"),
        {
            materials.get_opaque("Crystal_mat")
        },
        { transforms.transforms.insert(crystal_ball) }
    );

    //models.add_model("assets/models/InstanceTest/test.fbx");
    //Transform test;
    //test.set_world_offset({ -20.f, 15.f, 20.f });
    //test.set_scale({0.2f, 0.5f, 0.2f});
    //meshes.opaque_instances.add_model_instance(
    //    models.get_ptr("assets/models/InstanceTest/test.fbx"),
    //    {
    //    	materials.get_opaque("assets/models/InstanceTest/Cube_mat"),
    //        materials.get_opaque("assets/models/InstanceTest/Sphere_mat")
    //    },
    //    { transforms.transforms.insert(test) }
    //);

    Transform samurai;
    samurai.set_scale(5.f);
    samurai.set_world_offset({ 10.f, 0.f, 0.f });
    //meshes.opaque_instances.add_model_instance(
    //    models.get_ptr("assets/models/Samurai/Samurai.fbx"),
    //    {
    //        materials.get_opaque("assets/models/Samurai/Sword_mat"),
    //        materials.get_opaque("assets/models/Samurai/Head_mat"),
    //        materials.get_opaque("assets/models/Samurai/Eyes_mat"),
    //        materials.get_opaque("assets/models/Samurai/Helmet_mat"),
    //        materials.get_opaque("assets/models/Samurai/Skirt_mat"),
    //        materials.get_opaque("assets/models/Samurai/Legs_mat"),
    //        materials.get_opaque("assets/models/Samurai/Hands_mat"),
    //        materials.get_opaque("assets/models/Samurai/Torso_mat")
    //    },
    //    { transforms.transforms.insert(samurai) }
    //);

    //samurai.set_scale(5.f);
    //samurai.set_world_offset({ 5.f, 0.f, 3.f });
    //meshes.opaque_instances.add_model_instance(
    //    models.get_ptr("assets/models/Samurai/Samurai.fbx"),
    //    {
    //        materials.get_opaque("assets/models/Samurai/Sword_mat"),
    //        materials.get_opaque("assets/models/Samurai/Head_mat"),
    //        materials.get_opaque("assets/models/Samurai/Eyes_mat"),
    //        materials.get_opaque("assets/models/Samurai/Helmet_mat"),
    //        materials.get_opaque("assets/models/Samurai/Skirt_mat"),
    //        materials.get_opaque("assets/models/Samurai/Legs_mat"),
    //        materials.get_opaque("assets/models/Samurai/Hands_mat"),
    //        materials.get_opaque("assets/models/Samurai/Torso_mat")
    //    },
    //    { transforms.transforms.insert(samurai) }
    //);

    //Transform knight;
    //knight.set_scale(5.f);
    //knight.set_world_offset({ 20.f, 0.f, 0.f });
    //meshes.opaque_instances.add_model_instance(
    //    models.get_ptr("assets/models/Knight/Knight.fbx"),
    //    {
    //        materials.get_opaque("assets/models/Knight/Fur_mat"),
    //    	materials.get_opaque("assets/models/Knight/Legs_mat"),
    //    	materials.get_opaque("assets/models/Knight/Torso_mat"),
    //    	materials.get_opaque("assets/models/Knight/Head_mat"),
    //    	materials.get_opaque("assets/models/Knight/Eyes_mat"),
    //    	materials.get_opaque("assets/models/Knight/Helmet_mat"),
    //    	materials.get_opaque("assets/models/Knight/Skirt_mat"),
    //    	materials.get_opaque("assets/models/Knight/Cape_mat"),
    //    	materials.get_opaque("assets/models/Knight/Gloves_mat")
    //    },
    //    { transforms.transforms.insert(knight) }
    //);

    //knight.set_scale(5.f);
    //knight.set_world_offset({ 15.f, 0.f, 3.f });
    //meshes.opaque_instances.add_model_instance(
    //    models.get_ptr("assets/models/Knight/Knight.fbx"),
    //    {
    //        materials.get_opaque("assets/models/Knight/Fur_mat"),
    //        materials.get_opaque("assets/models/Knight/Legs_mat"),
    //        materials.get_opaque("assets/models/Knight/Torso_mat"),
    //        materials.get_opaque("assets/models/Knight/Head_mat"),
    //        materials.get_opaque("assets/models/Knight/Eyes_mat"),
    //        materials.get_opaque("assets/models/Knight/Helmet_mat"),
    //        materials.get_opaque("assets/models/Knight/Skirt_mat"),
    //        materials.get_opaque("assets/models/Knight/Cape_mat"),
    //        materials.get_opaque("assets/models/Knight/Gloves_mat")
    //    },
    //    { transforms.transforms.insert(knight) }
    //);

    //Transform horse;
    //horse.set_scale(5.f);
    //horse.set_world_offset({ 30.f, 0.f, 0.f });
    //meshes.opaque_instances.add_model_instance(
    //    models.get_ptr("assets/models/KnightHorse/KnightHorse.fbx"),
    //    {
    //        materials.get_opaque("assets/models/KnightHorse/Armor_mat"),
    //        materials.get_opaque("assets/models/KnightHorse/Horse_mat"),
    //        materials.get_opaque("assets/models/KnightHorse/Tail_mat")
    //    },
    //    { transforms.transforms.insert(horse) }
    //);

    //horse.set_scale(5.f);
    //horse.set_world_offset({ 25.f, 0.f, 5.f });
    //meshes.opaque_instances.add_model_instance(
    //    models.get_ptr("assets/models/KnightHorse/KnightHorse.fbx"),
    //    {
    //        materials.get_opaque("assets/models/KnightHorse/Armor_mat"),
    //        materials.get_opaque("assets/models/KnightHorse/Horse_mat"),
    //        materials.get_opaque("assets/models/KnightHorse/Tail_mat")
    //    },
    //    { transforms.transforms.insert(horse) }
    //);

    Transform wall;
    wall.set_scale(5.f);
    wall.set_world_offset({ 0.f, 0.f, 15.f });
    meshes.opaque_instances.add_model_instance(
        models.get_ptr("assets/models/SunCityWall/SunCityWall.fbx"),
        {
            materials.get_opaque("assets/models/SunCityWall/Star_mat"),
            materials.get_opaque("assets/models/SunCityWall/Wall_mat"),
            materials.get_opaque("assets/models/SunCityWall/Trims_mat"),
            materials.get_opaque("assets/models/SunCityWall/Statue_mat"),
            materials.get_opaque("assets/models/SunCityWall/Stonework_mat"),
        }, 
        { transforms.transforms.insert(wall) }
    );

    wall.set_scale(5.f);
    wall.set_world_offset({ -15.82f, 0.f, 15.f });
    meshes.opaque_instances.add_model_instance(
        models.get_ptr("assets/models/SunCityWall/SunCityWall.fbx"),
        {
            materials.get_opaque("assets/models/SunCityWall/Star_mat"),
            materials.get_opaque("assets/models/SunCityWall/Wall_mat"),
            materials.get_opaque("assets/models/SunCityWall/Trims_mat"),
            materials.get_opaque("assets/models/SunCityWall/Statue_mat"),
            materials.get_opaque("assets/models/SunCityWall/Stonework_mat"),
        },
        { transforms.transforms.insert(wall) }
    );

    SmokeEmitter emitter {vec3f{0.f, 0.f, 5.f}, 0.5f, 1.f};
    emitter.particleLifetime = 10.f;
    emitter.particleInitSize = {2.f, 2.f, 2.f};
    particle_system.add_smoke_emitter( emitter );

    emitter = {vec3f{-15.f, 0.f, 5.f}, 0.25f, 2.f};
    emitter.particleInitTint = {1.f, 0.2f, 0.5f};
	emitter.particleLifetime = 10.f;
    emitter.particleInitSize = {2.f, 2.f, 2.f};
    particle_system.add_smoke_emitter( emitter );

    init_floor(16, 16);

    scene.init_hdr_and_depth_buffer(window.width(), window.height(), 4);
    scene.init_depth_stencil_state();
    scene.grassfield.grassBuffer = {3u, 4u, {2.f, 3.f}};
    scene.grassfield.init_field({0.f, 0.f, 0.f}, 10.f, 10.f, 1.f);
    
    camera.set_world_offset({ 0.f, 15.f, -10.f });
    camera.set_perspective(rad(55.f), float(window.width()) / window.height(), 0.1f, 400.f);

    postProcess.postProcessShader = shaders.get_ptr(L"shaders/resolve.hlsl");
    postProcess.postProcessShaderMS = shaders.get_ptr(L"shaders/resolve_ms.hlsl");
    postProcess.ev100 = 0.f;

    lights.init_depth_buffers(1024);
}

void Controller::start_appearance()
{
    MeshSystem& mesh_system = MeshSystem::instance();
    ModelManager& model_manager = ModelManager::instance();
    MaterialManager& material_manager = MaterialManager::instance();
    TransformSystem& transform_system = TransformSystem::instance();

    Transform subsamurai;
    subsamurai.set_scale(5.f);
    subsamurai.add_world_offset({ -10.f, 0.f, 0.f });
     mesh_system.appearing_instances.add_model_instance(
        model_manager.get_ptr("assets/models/Samurai/Samurai.fbx"),
        {
            material_manager.get_opaque("assets/models/Samurai/Sword_mat"),
            material_manager.get_opaque("assets/models/Samurai/Head_mat"),
            material_manager.get_opaque("assets/models/Samurai/Eyes_mat"),
            material_manager.get_opaque("assets/models/Samurai/Helmet_mat"),
            material_manager.get_opaque("assets/models/Samurai/Skirt_mat"),
            material_manager.get_opaque("assets/models/Samurai/Legs_mat"),
            material_manager.get_opaque("assets/models/Samurai/Hands_mat"),
            material_manager.get_opaque("assets/models/Samurai/Torso_mat")
		},
        {
            transform_system.transforms.insert(subsamurai), vec3f{0.7f, 1.5f, 5.f}, 5.f
        }
    );
}

void Controller::update_appearance_instances(float dt)
{
	AppearingInstances& appearingInstances = MeshSystem::instance().appearing_instances;

	for (uint32_t modelInd = 0; modelInd < appearingInstances.perModels.size(); modelInd++)
	{
		auto& model = appearingInstances.perModels.at(modelInd);
		for(uint32_t instanceInd = model.instances.size(); instanceInd > 0; instanceInd--)
		{
			AppearingInstances::Instance& instance = model.instances.at(instanceInd - 1);
			instance.animationPassed += dt;
			if(instance.animationPassed > instance.animationTime)
			{
                {
	                std::vector<OpaqueMaterial> materials{model.perMeshes.size()};

	                for(uint32_t meshInd = 0; meshInd < model.perMeshes.size(); meshInd++)
						materials[meshInd] = model.perMeshes[meshInd].perMaterials[instance.materials[meshInd]].material;

                    MeshSystem::instance().opaque_instances.add_model_instance( 
                        model.model, std::move(materials), {instance.model_world}
					);
                }

				appearingInstances.remove_model_instance(modelInd, instanceInd);
			}
		}
	}
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
    
    static int msaa = 2;
    if (ImGui::SliderInt("MSAA", &msaa, 1, 3))
        scene.init_hdr_and_depth_buffer(scene.hdr_buffer.width, scene.hdr_buffer.height, 1 << msaa);

    ImGui::SameLine();
    ImGui::Text("%d", 1 << msaa);

    ImGui::End();
}

void Controller::OnResize(uint32_t width, uint32_t height)
{
    scene.init_hdr_and_depth_buffer(width, height, scene.hdr_buffer.msaa);
    scene.init_depth_stencil_buffer(width, height);
    camera.change_aspect(float(width) / height);
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

    if (is.keyboard.keys[PLUS])  postProcess.ev100 += 0.1f;
    if (is.keyboard.keys[MINUS]) postProcess.ev100 -= 0.1f;

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
    if (is.keyboard.keys[N] && !is.keyboard.previus_key_state[N])
    {
	    start_appearance();
        is.keyboard.previus_key_state[N] = true;
    } else if(!is.keyboard.keys[N]) is.keyboard.previus_key_state[N] = false;

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
        up = camera.frustrum_up;
        right = camera.frustrum_right;

        dx = (is.mouse.x + 0.5f) / screen_width;
        dy = 1.f - (is.mouse.y + 0.5f) / screen_height;

        mouse_ray.origin = camera.position();
        mouse_ray.direction = ((camera.blnear_fpoint + right * dx + up * dy).head<3>() - mouse_ray.origin).normalized();

        record.intersection.reset(camera.zn, camera.zf);
        MeshSystem::instance().select_mesh(mouse_ray, record);

        is.mouse.rmb = MDOWN;

    case MDOWN:
        if (record.intersection.valid())
        {
            h = 2.f * camera.zn / camera.proj(1, 1);
            w = camera.aspect * h;

        	prop = fabsf((record.intersection.point * camera.view.col(2).head<3>() + camera.view(3, 2)) / camera.zn);
            if(camera.fps_camera)
            {
                //FPS
                rotation = quatf{ Eigen::AngleAxisf{rot.pitch, vec3f{1.f, 0.f,0.f}} };
                rotation *= quatf{ Eigen::AngleAxisf{
                    rot.yaw,
                    vec3f{0,1,0} *camera.view.topLeftCorner<3, 3>()}
                };

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
                rotation = quatf{ Eigen::AngleAxisf{rot.roll, vec3f{0.f, 0.f,1.f}} };
                rotation *= quatf{ Eigen::AngleAxisf{rot.pitch, vec3f{1.f, 0.f,0.f}} };
                rotation *= quatf{ Eigen::AngleAxisf{rot.yaw, vec3f{0.f, 1.f,0.f}} };

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

            TransformSystem::instance().transforms[record.transormId].add_world_offset(offset);
        }

        break;
    case RELEASED:
        is.mouse.rmb = MUP;
    case MUP:
        break;
    }

    if(camera_update) move_camera(move, rot);

    is.mouse.prev_x = is.mouse.x;
    is.mouse.prev_y = is.mouse.y;

    ParticleSystem::instance().update(dt);
    update_appearance_instances(dt);
}
