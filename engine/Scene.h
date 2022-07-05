#pragma once
#include <vector>

#include "render/Direct3D.h"
#include "ParallelExecutor.h"
#include "Camera.h"
#include "ImageSettings.h"
#include "IntersectionQuery.h"
#include "math/CubeMesh.h"
#include "render/Material.h"
#include "render/Lighting.h"
#include "objects/MeshInstance.h"
#include "objects/SphereObject.h"
#include "objects/PlaneObject.h"
#include "objects/LightObjects.h"

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
	DirectLight dirlight;
	std::vector<Material> materials;
	std::vector<PointLightObject> point_lights;
	std::vector<SpotlightObject> spotlights;
	std::vector<SphereObject> spheres;
	std::vector<MeshInstance> meshes;
	std::vector<Mesh> meshes_data;

	comptr<ID3D11Buffer> vertexBuffer;
	UINT vertex_buffer_stride = 6 * sizeof(float);
	UINT vertex_buffer_offset = 0;

	void select_object(const Ray& ray, float t_min, float t_max, IntersectionQuery& record);
	
	void init_buffers();
};



