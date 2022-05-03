#pragma once
#include <vector>

#include "RenderObject.h"
#include "SphereObject.h"

class RenderList: public RenderObject
{
	std::vector<std::shared_ptr<SphereObject>> list;

public:

	void add(std::shared_ptr<SphereObject> ptr)
	{
		list.push_back(ptr);
	}

	void add(const SphereObject& object)
	{
		list.push_back(std::make_shared<SphereObject>(object));
	}

	void move(double dx, double dy, double dz)
	{
		for (int i = 0; i < list.size(); ++i)
		{
			RenderObject& obj = *list.at(i);
			obj.move(dx, dy, dz);
		}
	}

	void move(const Vec3& vec) override
	{
		for (int i = 0; i < list.size(); ++i)
		{
			RenderObject& obj = *list.at(i);
			obj.move(vec);
		}
	}

	bool ray_collision(const Ray& ray, double t_min, hit_record& record) const override
	{
		hit_record result;
		double prev_z = DBL_MAX;
		bool hit = false;

		for (int i = 0; i < list.size(); ++i)
		{
			RenderObject& obj = *list.at(i);
			if (obj.ray_collision(ray, t_min, result) && result.point.z < prev_z)
			{
				prev_z = result.point.z;
				record = result;
				hit = true;
			}
		}
		return hit;
	}

	std::shared_ptr<RenderObject> choose_object(const Ray& ray, double t_min)
	{
		hit_record result;
		double prev_z = DBL_MAX;
		std::shared_ptr<RenderObject> chosen;

		for (int i = 0; i < list.size(); ++i)
		{
			RenderObject& obj = *list.at(i);
			if (obj.ray_collision(ray, t_min, result) && result.point.z < prev_z)
			{
				prev_z = result.point.z;
				chosen = list.at(i);
			}
		}
		return chosen;
	}
};
