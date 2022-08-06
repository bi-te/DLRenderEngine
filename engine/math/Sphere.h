#pragma once

#include "Intersection.h"
#include "Ray.h"

class Sphere
{
public:

	vec3f center;
	float radius;

	Sphere(const vec3f& origin = { 0, 0, 0 }, float radius = 0)
		: center(origin),
		  radius(radius)
	{
	}

	bool intersection(const Ray& ray, float t_min, float t_max, Intersection& record) const;
	
};