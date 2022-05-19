#pragma once
#define PI 3.14159265

#include "Eigen/Core"
#include "Eigen/Geometry"

using mat4 = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;
using mat3 = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;
using vec4 = Eigen::RowVector4f;
using vec3 = Eigen::RowVector3f;
using quat = Eigen::Quaternionf;

using uint = unsigned int;

struct Angles
{
	float roll, pitch, yaw;
};

inline float to_radians(float degrees)
{
	return degrees * PI / 180.f;
}
	
inline Angles rotation_from_degrees(float roll, float pitch, float yaw)
{
	float r = to_radians(roll);
	float p = to_radians(pitch);
	float y = to_radians(yaw);

	return { r, p, y };
}