#pragma once

#include "math/Ray.h"

struct hit_record
{
	double t;
	Point3d point;
	Vec3 norm;
};

class RenderObject
{
public:
	virtual void move(double dx, double dy, double dz) = 0;
	virtual void move(const Vec3& vec) = 0;
	virtual bool ray_collision(const Ray& ray, double t_min, hit_record& record) const = 0;
};