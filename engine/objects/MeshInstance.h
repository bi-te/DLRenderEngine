#pragma once

#include "../math/Mesh.h"
#include "../math/Ray.h"
#include "../math/Transform.h"
#include "../math/Intersection.h"

class MeshInstance
{
public:
	uint32_t material;
	Mesh* mesh;
	Transform transform;

	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& nearest, uint32_t& material_index) const;
	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& record) const;
};
