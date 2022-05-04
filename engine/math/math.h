#pragma once

#include "Vec3.h"

inline float length_squared(const Vec3& vec)
{
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline float length(const Vec3& vec)
{
	return sqrt(length_squared(vec));
}

inline void make_unit_vector(Vec3& vec)
{
	float l = length(vec);
	vec.x /= l;
	vec.y /= l;
	vec.z /= l;
}