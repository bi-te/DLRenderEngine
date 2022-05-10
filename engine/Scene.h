#pragma once
#include <vector>

#include "Camera.h"
#include "Material.h"
#include "math/Lighting.h"
#include "Screen.h"
#include "SphereObject.h"
#include "math/Ray.h"

const Vec3 AMBIENT{ 50, 50, 50 };

class Scene
{
public:
	DirectLight sunlight;
	std::vector<Material> materials;
	std::vector<PointLight> point_lights;
	std::vector<Spotlight> spotlights;
	std::vector<SphereObject> spheres;
	

	bool ray_collision(const Ray& ray, float t_min, float t_max, Intersection& record) const;

	void calc_direct_light(Vec3& color, Intersection& record) const;
	void calc_point_lights(Vec3& color, Intersection& record) const;
	void calc_spotlights(Vec3& color, Intersection& record) const;

	void draw(Screen& screen, const Camera& camera);
};
