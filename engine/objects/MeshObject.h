#pragma once

#include "../math/math.h"
#include "MeshInstance.h"

class MeshObject
{
public:
	uint material;
	MeshInstance cube;

	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& nearest, uint& material_index) const
	{
		Intersection record = Intersection::infinite();
		cube.intersection(ray, t_min, t_max, record);

		if (record.t >= nearest.t)return false;

		nearest = record;
		material_index = material;
		return true;
	}
};
