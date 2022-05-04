#include "Sphere.h"

bool Sphere::intersection(const Ray& ray, float t_min, hit_record& record) const
{
	Vec3 oc = ray.get_origin() - center;
	float b = 2 * (ray.get_direction() * oc);
	float c = oc * oc - pow(radius, 2);

	float D = b * b - 4 * c;
	if (D > 0)
	{
		float t = (-b - sqrt(D)) / 2;
		if (t > t_min)
		{
			record.t = t;
			record.point = ray.position(t);
			record.norm = (record.point - center) / radius;
			return true;
		}
	}
	return false;
}
