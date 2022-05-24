#pragma once
#include "math/Intersection.h"
#include "moving/ObjectMover.h"

struct IntersectionQuery
{
	Intersection intersection;
	std::unique_ptr<ObjectMover> mover;
};