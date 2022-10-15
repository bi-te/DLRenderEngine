#pragma once

#include "math.h"

inline Angles rotation_from_degrees(float roll, float pitch, float yaw)
{
	float r = rad(roll);
	float p = rad(pitch);
	float y = rad(yaw);

	return { r, p, y };
}
inline vec3f lerp(const vec3f& v1, const vec3f& v2, float t)
{
	return vec3f{
		v1.x() + t * (v2.x() - v1.x()),
		v1.y() + t * (v2.y() - v1.y()),
		v1.z() + t * (v2.z() - v1.z())
	};
}

inline vec3f reflect(const vec3f& incident, const vec3f& normal)
{
	return incident + 2 * normal.dot(-incident) * normal;
}

inline float smoothstep(float edge0, float edge1, float x)
{
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
	return x * x * (3.f - 2.f * x);
}

inline void clamp_to_horizon(const vec3f& norm, vec3f& dir, float min_cos)
{
	float cosNoD = norm.dot(dir);
	if (cosNoD < 0.f)
		dir = (dir + norm * (min_cos - cosNoD)).normalized();
}

void onb_frisvad(vec3f& b1, const vec3f& normal, vec3f& b2);
void onb_frisvad(mat3f& basis);
vec3f fibonacci_set_point(uint32_t number, float start_phi, uint32_t index);
std::vector<vec3f> fibonacci_set(uint32_t number, float start_phi);
void fibonacci_set(std::vector<vec3f>& set, float start_phi);

vec3f closest_sphere_direction(const vec3f& sphere_rel_pos, const vec3f& sphere_dir, const vec3f& reflection,
	float sphere_dist, float radius, float cos_sphere);

std::vector<vec2f> poisson_float_2d(float width, float height, float radius, vec2f center = {0.f, 0.f}, uint32_t k = 30);

