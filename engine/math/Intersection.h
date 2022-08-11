#pragma once
#include <limits>

#include "math.h"

struct Intersection
{
	float t;
	vec3f point;
	vec3f norm;

	Intersection() = default;

	void reset() { t = std::numeric_limits<float>::infinity(); }
	bool exists() const { return std::isfinite(t); }

	static Intersection infinite()
	{
		Intersection inf{};
		inf.reset();
		return inf;
	}
};
