#pragma once
#include <vector>

#include "ParallelExecutor.h"
#include "Camera.h"
#include "ImageSettings.h"
#include "IntersectionQuery.h"
#include "Screen.h"
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
	PlaneObject floor;
	CubeMesh cube;

	void select_object(const Ray& ray, float t_min, float t_max, IntersectionQuery& record);
	bool ray_light_collision(const Ray& ray, float t_min, float t_max, Intersection& nearest, uint32_t& material_index) const;
	bool ray_object_collision(const Ray& ray, float t_min, float t_max, Intersection& nearest, uint32_t& material_index) const;
	bool shadow_test(const Ray& ray, float t_max) const;
	void integral_test(const Ray& ray, float t_max, vec3& light) const;

	void process_direct_light(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const;
	void process_point_lights(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const;
	void process_spotlights(  vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const;
	
	void draw(Screen& screen, ImageSettings& image, const Camera& camera) const;
	void draw(Screen& screen, ImageSettings& image, const Camera& camera, ParallelExecutor& executor) const;
	void draw_pixel(Screen& screen, ImageSettings& image, const Camera& camera, uint32_t row, uint32_t column) const;

	vec3 reflection(Ray& ray, uint8_t depth, uint8_t max_depth, float t_max) const;
	void light_integral(vec3& color, const vec3& camera_pos, const Material& material, const Intersection& record, uint32_t tests)const;
};
