#pragma once
#define PI 3.14159265

#include "Vec3.h"

struct Angles
{
	float roll, pitch, yaw;
};

inline float to_radians(float degrees)
{
	return degrees * PI / 180.f;
}

inline float dot(const Vec3& rv, const Vec3& lv)
{
	return rv.x * lv.x + rv.y * lv.y + rv.z * lv.z;
}

inline float length_squared(const Vec3& vec)
{
	return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline float length(const Vec3& vec)
{
	return sqrt(length_squared(vec));
}

inline Vec3 normalize(const Vec3& vec)
{
	Vec3 norm{};
	float l = length(vec);
	norm.x = vec.x / l;
	norm.y = vec.y / l;
	norm.z = vec.z / l;

	return norm;
}