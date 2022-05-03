#pragma once

#include "Vec3.h"

class Sphere
{
protected:
	Point3d center;
	double radius;

public:
	Sphere(const Point3d& origin = { 0, 0, 0 }, double radius = 0)
		: center(origin),
		  radius(radius)
	{
	}

	void set_center(const Point3d& center)
	{
		this->center = center;
	}

	Point3d get_center() const
	{
		return center;
	}

	double get_radius() const
	{
		return radius;
	}
};