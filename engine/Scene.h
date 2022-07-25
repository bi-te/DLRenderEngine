#pragma once
#include <vector>

#include "Camera.h"
#include "ImageSettings.h"
#include "IntersectionQuery.h"
#include "render/Material.h"
#include "objects/MeshInstance.h"
#include "objects/Skybox.h"
#include "objects/SphereObject.h"
#include "render/Renderer.h"


const uint16_t MAX_REFLECTION_DEPTH = 2;	
const float MAX_REFLECTIVE_ROUGHNESS = 0.1f;
const float MAX_PROCESS_DISTANCE = 500.f;

const vec3 AMBIENT{ 0.18f, 0.f, 0.72f };

class Scene
{
	enum ObjType { SPHERE, MESH, POINTLIGHT, SPOTLIGHT, NONE };

	struct objectRef
	{
		void* ptr;
		ObjType type = NONE;
	};

public:
	Skybox skybox;
	std::vector<Material> materials;
	std::vector<MeshInstance> instances;
	std::vector<Mesh> meshes;

	bool select_object(const Ray& ray, float t_min, float t_max, IntersectionQuery& record);
	
	void init_objects_buffers();
	void reset_objects_buffers();

	void draw(const Camera& camera, Renderer& renderer);
};



