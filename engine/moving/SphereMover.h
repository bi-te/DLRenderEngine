#pragma once

#include "ObjectMover.h"
#include "../math/Sphere.h"

class SphereMover: public ObjectMover
{
public:
	Sphere& sphere;

	SphereMover(Sphere& sphere): sphere(sphere)
	{
	}

	void move(const vec3f& offset) override
	{
		sphere.center += offset;
	}
};
