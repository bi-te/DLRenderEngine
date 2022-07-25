#include "Scene.h"

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/ImGuiManager.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "moving/SphereMover.h"
#include "moving/PointLightMover.h"
#include "moving/SpotlightMover.h"
#include "moving/TransformMover.h"
#include "render/Direct11/Direct3D.h"

bool Scene::select_object(const Ray& ray, float t_min, float t_max, IntersectionQuery& record)
{
	bool intersection = false;
	objectRef ref;
	record.intersection = Intersection::infinite();

	for (MeshInstance& mesh : instances)
		if (mesh.intersection(ray, t_min, t_max, record.intersection)){
			ref.type = MESH;
			ref.ptr = &mesh;
			intersection = true;
		}

	switch (ref.type)
	{
	case SPHERE: record.mover = std::make_unique<SphereMover>(static_cast<SphereObject*>(ref.ptr)->sphere); break;
	case MESH: record.mover = std::make_unique<TransformMover>(static_cast<MeshInstance*>(ref.ptr)->transform); break;
	case POINTLIGHT: record.mover = std::make_unique<PointLightMover>(*static_cast<PointLightObject*>(ref.ptr)); break;
	case SPOTLIGHT: record.mover = std::make_unique<SpotlightMover>(*static_cast<SpotlightObject*>(ref.ptr)); break;
	case NONE: record.mover = nullptr;
	}

	return intersection;
}

void Scene::init_objects_buffers()
{
	for (auto & instance : instances)
	{
		instance.load_buffers();
	}
}

void Scene::reset_objects_buffers()
{
	for(auto& instance: instances)
	{
		instance.reset_buffers();
	}
}

void Scene::draw(const Camera& camera, Renderer& renderer)
{	
	renderer.clear_buffers(AMBIENT.data());
	renderer.prepare_output();
	renderer.bind_globals(camera.view_proj);

	for (MeshInstance & instance : instances)
	{
		instance.update_transform_buffer();
		instance.draw();
	}

	skybox.update_frustrum_buffer({
camera.blnear_fpoint - vec4{camera.position().x(), camera.position().y(), camera.position().z(), 1.f},
camera.frustrum_up, camera.frustrum_right
		});
	skybox.draw();

	if(ImGuiManager::active())
		ImGuiManager::flush();

	renderer.flush();
}
