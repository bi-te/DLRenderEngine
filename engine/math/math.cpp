#include "math.h"

std::vector<vec3> fibonacci_set(uint32_t number, float start_phi)
{
	std::vector<vec3> set;

	float dy = 1.f / number;
	float y = 1.f - dy / 2.f;

	vec3 point;
	float radius, phi = start_phi;
	for (uint32_t i = 0; i < number; ++i)
	{
		point.y() = y;

		radius = sqrtf(1.f - y * y);

		point.x() = cosf(phi) * radius;
		point.z() = sinf(phi) * radius;

		y -= dy;
		phi += GOLDEN_ANGLE;
		set.push_back(point);
	}

	return set;
}

void fibonacci_set(std::vector<vec3>& set, float start_phi)
{
	float dy = 1.f / set.size();
	float y = 1.f - dy / 2.f;

	vec3 point;
	float radius, phi = start_phi;
	for (uint32_t i = 0; i < set.size(); ++i)
	{
		point.y() = y;

		radius = sqrtf(1.f - y * y);

		point.x() = cosf(phi) * radius;
		point.z() = sinf(phi) * radius;

		y -= dy;
		phi += GOLDEN_ANGLE;
		set.at(i) = point;
	}

}

vec3 fibonacci_set_point(uint32_t number, float start_phi, uint32_t index)
{
	vec3 point;

	point.y() = 1.f - (index + 0.5f) / number;
	float radius = sqrtf(1.f - point.y() * point.y());
	float phi = start_phi + GOLDEN_ANGLE * index;

	point.x() = cosf(phi) * radius;
	point.z() = sinf(phi) * radius;
	return point;
}

void onb_frisvad(vec3& b1, const vec3& normal, vec3& b2)
{
	float sign = copysignf(1.0f, normal.z());
	const float a = -1.0f / (sign + normal.z());
	const float b = normal.x() * normal.y() * a;
	b1 = vec3(1.0f + sign * normal.x() * normal.x() * a, sign * b, -sign * normal.x());
	b2 = vec3(b, sign + normal.y() * normal.y() * a, -normal.y());
}

void onb_frisvad(mat3& basis)
{
	auto n = basis.row(1);
	float sign = copysignf(1.0f, n.z());
	const float a = -1.0f / (sign + n.z());
	const float b = n.x() * n.y() * a;
	basis.row(0) = vec3(1.0f + sign * n.x() * n.x() * a, sign * b, -sign * n.x());
	basis.row(2) = -vec3(b, sign + n.y() * n.y() * a, -n.y());
}

vec3 closest_sphere_direction(const vec3& sphere_rel_pos, const vec3& sphere_dir, const vec3& reflection,
	float sphere_dist, float radius, float cos_sphere)
{
	float cosRoS = reflection.dot(sphere_dir);

	if (cosRoS >= cos_sphere) return reflection;
	if (cosRoS < 0.f) return sphere_dir;

	vec3 closes_point_dir = (reflection * sphere_dist * cosRoS - sphere_rel_pos).normalized();
	return (sphere_rel_pos + closes_point_dir * radius).normalized();
}
