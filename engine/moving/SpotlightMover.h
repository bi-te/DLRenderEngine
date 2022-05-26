#pragma once
#pragma once

#include "ObjectMover.h"
#include "../objects/LightObjects.h"

class SpotlightMover : public ObjectMover
{
public:
	SpotlightObject& plobject;

	SpotlightMover(SpotlightObject& plobject) : plobject(plobject)
	{
	}

	void move(const vec3& offset) override
	{
		plobject.sphere.center += offset;
		plobject.spotlight.position += offset;
	}

};