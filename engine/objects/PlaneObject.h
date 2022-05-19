#pragma once

#include "../math/Plane.h"

class PlaneObject
{
public:
	Plane plane;
	uint material;

	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& nearest, uint& material_index) const
	{
		if (plane.intersection(ray, t_min, t_max, nearest)) {
			material_index = material;
			return true;
		}

		return false;
	}
};
