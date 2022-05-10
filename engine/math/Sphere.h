#pragma once

#include "Ray.h"

class Sphere
{
public:

	Point3d center;
	float radius;

	Sphere(const Point3d& origin = { 0, 0, 0 }, float radius = 0)
		: center(origin),
		  radius(radius)
	{
	}

	bool intersection(const Ray& ray, float& t) const;
	
};