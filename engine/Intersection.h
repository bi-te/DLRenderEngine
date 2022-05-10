#pragma once
#include <limits>

#include "math/Vec3.h"

struct Intersection
{
	float t;
	Point3d point;
	Vec3 norm;
	unsigned int material;

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
