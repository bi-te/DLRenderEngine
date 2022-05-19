#pragma once

#include "../math/math.h"

class ObjectMover
{
public:
	virtual void move(const vec3& offset) = 0;
};

