#pragma once
#include "math.h"

struct hit_record
{
	float t;
	Point3d point;
	Vec3 norm;
};

class Ray
{
	Point3d origin;
	Vec3 direction;
public:
	Ray(): origin(0, 0, 0), direction(0, 0, 0)
	{
		
	}

	Ray(const Point3d& origin, const Vec3& direction)
		: origin(origin), direction(direction)
	{
		make_unit_vector(this->direction);
	}

	void set_origin(const Point3d& origin)
	{
		this->origin = origin;
	}

	Point3d get_origin() const
	{
		return origin;
	}

	void set_direction(const Vec3& direction)
	{
		this->direction = direction;
	}

	Vec3 get_direction() const
	{
		return direction;
	}

	Point3d position(float k) const
	{
		return origin + direction * k;
	}
};