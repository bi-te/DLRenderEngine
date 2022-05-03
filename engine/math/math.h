#pragma once

#include "Vec3.h"

inline double length_squared(const Vec3& vec)
{
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline double length(const Vec3& vec)
{
	return sqrt(length_squared(vec));
}

inline void make_unit_vector(Vec3& vec)
{
	double l = length(vec);
	vec.x /= l;
	vec.y /= l;
	vec.z /= l;
}