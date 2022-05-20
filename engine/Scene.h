#pragma once
#include <vector>

#include "Camera.h"
#include "Screen.h"
#include "math/CubeMesh.h"
#include "render/Material.h"
#include "render//Lighting.h"
#include "objects/MeshInstance.h"
#include "objects/SphereObject.h"
#include "objects/PlaneObject.h"
#include "objects/LightObjects.h"
#include "moving/ObjectMover.h"

const vec3 AMBIENT{ 70, 70, 70 };

class Scene
{
public:
	DirectLight sunlight;
	std::vector<Material> materials;
	std::vector<PointLightObject> point_lights;
	std::vector<SpotlightObject> spotlights;
	std::vector<SphereObject> spheres;
	std::vector<MeshInstance> meshes;
	PlaneObject floor;
	CubeMesh cube;

	std::unique_ptr<ObjectMover> select_object(const Ray& ray, float t_min, float t_max, Intersection& record);
	bool ray_collision(const Ray& ray, float t_min, float t_max, Intersection& nearest, uint& material_index) const;
	bool shadow_test(const Ray& ray, float t_min, float t_max) const;

	void process_direct_light(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const;
	void process_point_lights(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const;
	void process_spotlights(  vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const;

	void draw(Screen& screen, const Camera& camera);
};
