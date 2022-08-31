#include "Ray.h"

#include "Intersection.h"

bool Ray::intersect(const vec3f& v1, const vec3f& v2, const vec3f& v3, Intersection& nearest) const
{
	vec3f ab = v2 - v1;
	vec3f ac = v3 - v1;
	vec3f ao = origin - v1;

	vec3f norm = ab.cross(ac);

	float det = -direction.dot(norm);

	float u = -direction.dot(ao.cross(ac)) / det;
	if (u < 0 || u > 1) return false;

	float v = -direction.dot(ab.cross(ao)) / det;
	if (v < 0 || v + u > 1) return false;

	float t = ao.dot(norm) / det;

	if (t > nearest.near_limit && t < nearest.far_limit && t < nearest.t)
	{
		nearest.t = t;
		nearest.point = position(t);
		nearest.norm = norm.normalized();
		return true;
	}

	return false;
}
