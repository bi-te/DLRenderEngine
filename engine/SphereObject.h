#pragma once

#include "RenderObject.h"
#include "math/Sphere.h"

class SphereObject: public Sphere, public RenderObject
{

public:
	SphereObject() = default;

	SphereObject(const Point3d& origin = { 0, 0, 0 }, double radius = 0) : Sphere(origin, radius)
	{
	}

	void move(double dx, double dy, double dz) override
	{
		center.x += dx;
		center.y += dy;
		center.z += dz;
	}

	void move(const Vec3& vec) override
	{
		center += vec;
	}

	bool ray_collision(const Ray& ray, double t_min, hit_record& record) const override
	{
		Vec3 oc = ray.get_origin() - center;
		double b = 2 * (ray.get_direction() * oc);
		double c = oc * oc - pow(radius, 2);

		double D = b * b - 4 * c;
		if(D > 0)
		{
			double t = (-b - sqrt(D)) / 2;
			if(t  > t_min)
			{
				record.t = t;
				record.point = ray.position(t);
				record.norm = (record.point - center) / radius;
				return true;
			}

			t = (-b + sqrt(D)) / 2;
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
};