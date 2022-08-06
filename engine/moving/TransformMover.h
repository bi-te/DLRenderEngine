#pragma once

#include "ObjectMover.h"
#include "../math/Transform.h"

class TransformMover: public ObjectMover
{
public:
	Transform& transform;
	TransformMover(Transform& transform) : transform(transform)
	{
	}

	void move(const vec3f& offset) override
	{
		transform.add_world_offset(offset);
		transform.update();
	}
};
