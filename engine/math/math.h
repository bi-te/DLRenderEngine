#pragma once

#include "Eigen/Core"
#include "Eigen/Geometry"

using mat4f = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;
using mat3f = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;
using vec4f = Eigen::RowVector4f;
using vec3f = Eigen::RowVector3f;
using vec2f = Eigen::RowVector2f;
using arr3f = Eigen::Array3f;
using arr4f = Eigen::Array3f;
using quatf = Eigen::Quaternionf;

const float PI = 3.1415926535f;
const float GOLDEN_ANGLE = 2.3999632297f;

struct Angles
{
	float roll, pitch, yaw;
};

inline float rad(float degrees)
{
	return degrees * PI / 180.f;
}
