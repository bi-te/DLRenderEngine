#pragma once

#include "vec3.h"

class sphere
{
private:
	point3d center;
	int32_t radius;

public:
	sphere(const point3d& origin = { 0, 0, 0 }, int32_t radius = 0)
		: center(origin),
		  radius(radius)
	{
	}

	void set_center(const point3d& center)
	{
		this->center = center;
	}

	point3d get_center() const
	{
		return center;
	}

	int32_t get_radius() const
	{
		return radius;
	}
};