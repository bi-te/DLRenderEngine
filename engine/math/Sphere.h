#pragma once

#include "Vec3.h"

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
	
};