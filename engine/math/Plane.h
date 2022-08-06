#pragma once

#include "Ray.h"

class Plane
{
public:
	vec3f point;
	vec3f normal;

	Plane() = default;

	Plane(const vec3f& normal, const vec3f& point={0, 0, 0})
		: point(point),
		  normal(normal)
	{
		this->normal.normalize();
	}

	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& record) const
	{
		float denom = normal.dot(-ray.direction);
		if(fabs(denom) < std::numeric_limits<double>::epsilon()	) return false;

		vec3f op = ray.origin - point;
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