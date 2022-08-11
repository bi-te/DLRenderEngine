#pragma once
#include "math.h"

class Ray
{
public:
	vec3f origin;
	vec3f direction;

	Ray(): origin(0, 0, 0), direction(0, 0, 0)
	{
		
	}

	Ray(const vec3f& origin, const vec3f& direction)
		: origin(origin), direction(direction)
	{
		this->direction.normalize();
	}

	vec3f position(float k) const
	{
		return origin + direction * k;
	}
};