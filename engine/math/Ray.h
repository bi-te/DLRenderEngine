#pragma once
#include "math.h"

class Ray
{
public:
	Point3d origin;
	Vec3 direction;

	Ray(): origin(0, 0, 0), direction(0, 0, 0)
	{
		
	}

	Ray(const Point3d& origin, const Vec3& direction)
		: origin(origin), direction(direction)
	{
		this->direction = normalize(this->direction);
	}

	Point3d position(float k) const
	{
		return origin + direction * k;
	}
};