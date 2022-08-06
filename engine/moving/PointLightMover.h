#pragma once

#include "ObjectMover.h"
#include "../objects/LightObjects.h"

class PointLightMover: public ObjectMover
{
public:
	PointLightObject& plobject;

	PointLightMover(PointLightObject& plobject):  plobject(plobject)
	{
	}

	void move(const vec3f& offset) override
	{
		plobject.sphere.center += offset;
		plobject.plight.position += offset;
	}

};