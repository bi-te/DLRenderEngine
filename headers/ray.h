#pragma once
#include "vec3.h"

class ray
{
private:
	point3d origin;
	vec3 direction;
public:
	ray(): origin(0, 0, 0), direction(0, 0, 0)
	{
		
	}

	ray(const point3d& origin, const vec3& direction)
		: origin(origin), direction(direction)
	{
	}

	void set_origin(const point3d& origin)
	{
		this->origin = origin;
	}

	point3d get_origin() const
	{
		return origin;
	}

	void set_direction(const vec3& direction)
	{
		this->direction = direction;
	}

	vec3 get_direction() const
	{
		return direction;
	}

	point3d position(double k)
	{
		return origin + direction * k;
	}
};