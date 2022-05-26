#pragma once

#include "Ray.h"

class Plane
{
public:
	vec3 point;
	vec3 normal;

	Plane() = default;

	Plane(const vec3& normal, const vec3& point={0, 0, 0})
		: point(point),
		  normal(normal)
	{
		this->normal.normalize();
	}

	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& record) const
	{
		float denom = normal.dot(-ray.direction);
		if(denom <= 0) return false;

		vec3 op = ray.origin - point;
		float t = normal.dot(op) / denom;

		if (t > t_min && t < t_max && t < record.t)
		{
			record.t = t;
			record.point = ray.position(t);
			record.norm = normal;
			return true;
		}

		return false;
		
	}
};