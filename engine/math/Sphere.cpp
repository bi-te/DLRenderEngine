#include "Sphere.h"

bool Sphere::intersection(const Ray& ray, float t_min, float t_max, Intersection& record) const
{
	vec3 oc = ray.origin - center;
	float b = 2 * ray.direction.dot(oc);
	float c = oc.dot(oc) - powf(radius, 2);

	float D = b * b - 4 * c;
	if (D < 0) return false;
	
	float t = (-b - sqrt(D)) / 2;
	if (t > t_min && t < t_max && t < record.t)
	{
		record.t = t;
		record.point = ray.position(t);
		record.norm = (record.point - center) / radius;
		return true;
	}
	return false;
}
