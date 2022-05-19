#pragma once
#include <limits>

#include "math.h"

struct Intersection
{
	float t;
	vec3 point;
	vec3 norm;

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
