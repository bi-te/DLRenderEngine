#pragma once

#include "Eigen/Core"
#include "Eigen/Geometry"

using mat4 = Eigen::Matrix<float, 4, 4, Eigen::RowMajor>;
using mat3 = Eigen::Matrix<float, 3, 3, Eigen::RowMajor>;
using vec4 = Eigen::RowVector4f;
using vec3 = Eigen::RowVector3f;
using arr3 = Eigen::Array3f;
using arr4 = Eigen::Array3f;
using quat = Eigen::Quaternionf;

const float PI = 3.1415926535f;
const float GOLDEN_ANGLE = 2.3999632297f;

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
inline vec3 lerp(const vec3& v1, const vec3& v2, float t)
{
	return vec3{
		v1.x() + t * (v2.x() - v1.x()),
		v1.y() + t * (v2.y() - v1.y()),
		v1.z() + t * (v2.z() - v1.z())
	};
}

inline vec3 reflect(const vec3& incident, const vec3& normal)
{
	return incident + 2 * normal.dot(-incident) * normal;
}

inline float smoothstep(float edge0, float edge1, float x)
{
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
	return x * x * (3.f - 2.f * x);
}

void onb_frisvad(vec3& b1, const vec3& normal, vec3& b2);
void onb_frisvad(mat3& basis);
vec3 fibonacci_set_point(uint32_t number, float start_phi, uint32_t index);
std::vector<vec3> fibonacci_set(uint32_t number, float start_phi);

vec3 closest_sphere_direction(const vec3& sphere_rel_pos, const vec3& sphere_dir, const vec3& reflection,
	float sphere_dist, float radius, float cos_sphere);

inline void clamp_to_horizon(const vec3& norm, vec3& dir, float min_cos)
{
	float cosNoD = norm.dot(dir);
	if(cosNoD < 0.f) 
		dir = (dir + norm * (min_cos - cosNoD)).normalized();
}