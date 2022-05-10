#pragma once
#include "Intersection.h"
#include "math/Sphere.h"

class SphereObject
{
public:
	Sphere sphere;
	unsigned int material;

	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& nearest) const
	{
		float t = std::numeric_limits<float>::infinity();
		sphere.intersection(ray, t);
		if (t > t_min && t < nearest.t && t < t_max)
		{
			nearest.t = t;
			nearest.point = ray.position(t);
			nearest.norm = (nearest.point - sphere.center) / sphere.radius;
			nearest.material = material;
			return true;
		}
		return false;
	}

};