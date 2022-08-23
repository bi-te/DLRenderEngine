#pragma once
#include <limits>

#include "math.h"

struct Intersection
{
	float t;
	vec3f point;
	vec3f norm;
	uint32_t transormId;
	//uint32_t triangle;
	float near_limit;
	float far_limit;

	Intersection() = default;

	bool exists() const { return std::isfinite(t); }

	static Intersection infinite()
	{
		Intersection inf{};
		inf.reset(0.f);
		return inf;
	}
	
	constexpr void reset(float near, float far = std::numeric_limits<float>::infinity())
	{
		near_limit = near;
		far_limit = far;
		t = std::numeric_limits<float>::infinity();
	}

	bool valid() const { return std::isfinite(t); }
};
