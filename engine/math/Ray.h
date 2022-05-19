#pragma once
#include "math.h"

class Ray
{
public:
	vec3 origin;
	vec3 direction;

	Ray(): origin(0, 0, 0), direction(0, 0, 0)
	{
		
	}

	Ray(const vec3& origin, const vec3& direction)
		: origin(origin), direction(direction)
	{
		this->direction.normalize();
	}

	vec3 position(float k) const
	{
		return origin + direction * k;
	}
};