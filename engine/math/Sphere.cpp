#include "Sphere.h"

bool Sphere::intersection(const Ray& ray, float& t) const
{
	Vec3 oc = ray.origin - center;
	float b = 2 * dot(ray.direction, oc);
	float c = dot(oc, oc) - pow(radius, 2);

	float D = b * b - 4 * c;
	if (D > 0)
	{
		t = (-b - sqrt(D)) / 2;
		return true;
	}
	return false;
}
