#pragma once

#include "math.h"
#include "Ray.h"
#include "Intersection.h"

struct BoundingBox
{
	vec3f min;
	vec3f max;

	static constexpr float Inf = std::numeric_limits<float>::infinity();
	static BoundingBox empty() { return  { { Inf, Inf, Inf }, { -Inf, -Inf, -Inf } }; }
	static BoundingBox unit() { return  { { -1.f, -1.f, -1.f }, { 1.f, 1.f, 1.f } }; }

	vec3f size() const { return max - min; }
	vec3f center() const { return (min + max) / 2.f; }
	float radius() const { return size().norm() / 2.f; }

	void reset()
	{
		constexpr float maxf = std::numeric_limits<float>::max();
		min = { maxf , maxf , maxf };
		max = -min;
	}

	void expand(const BoundingBox& other)
	{
		min = min.cwiseMin(other.min);
		max = max.cwiseMax(other.max);
	}

	void expand(const vec3f& point)
	{
		min = min.cwiseMin(point);
		max = max.cwiseMax(point);
	}

	bool contains(const vec3f& P)
	{
		return
			min[0] <= P[0] && P[0] <= max[0] &&
			min[1] <= P[1] && P[1] <= max[1] &&
			min[2] <= P[2] && P[2] <= max[2];
	}

	bool intersection(const Ray& ray, float& nearest) const 
	{
		vec3f t1 = (min - ray.origin).array() / ray.direction.array();
		vec3f t2 = (max - ray.origin).array() / ray.direction.array();

		vec3f tmin = t1.cwiseMin(t2);
		vec3f tmax = t1.cwiseMax(t2);

		if (tmin.x() > tmax.y() || tmax.x() < tmin.y()) return false;

		float tNear = std::max(tmin.x(), tmin.y());
		float tFar = std::min(tmax.x(), tmax.y());

		if (tmax.z() < tNear || tmin.z() > tFar) return false;

		tNear = std::max(tNear, tmin.z());
		tFar = std::min(tFar, tmax.z());

		if (tNear < 0) tNear = tFar;

		if(tNear < nearest)
		{
			nearest = tNear;
			return true;
		}

		return false;
	}
};