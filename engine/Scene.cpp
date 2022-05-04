#include "Scene.h"

bool Scene::ray_collision(const Ray& ray, float t_min, hit_record& record) const
{
	hit_record result{};
	float prev_z = DBL_MAX;
	bool hit = false;

	for (const Sphere& sphere : objects)
	{
		Vec3 oc = ray.get_origin() - sphere.center;
		float b = 2 * (ray.get_direction() * oc);
		float c = oc * oc - pow(sphere.radius, 2);

		float D = b * b - 4 * c;
		if (D > 0)
		{
			float t = (-b - sqrt(D)) / 2;
			if (t > t_min)
			{
				result.t = t;
				result.point = ray.position(t);
				result.norm = (record.point - sphere.center) / sphere.radius;
				hit = true;
			}
		}

		if (result.point.z < prev_z)
		{
			prev_z = result.point.z;
			record = result;
		}
	}

	return hit;
}

void Scene::draw(Screen& screen)
{
	Ray ray;
	ray.set_direction({ 0, 0, 1 });

	uint16_t screen_width = screen.width();
	uint16_t screen_height = screen.height();
	hit_record record;


	for (int i = 0; i < screen_width; ++i)
	{
		for (int k = 0; k < screen_height; ++k)
		{
			ray.set_origin({ float(i),float(k), 0 });

			if (ray_collision(ray, 0, record))
			{
				screen[k * screen_width + i] = { 255, 255, 255 };
			}
			else
			{
				screen[k * screen_width + i] = { 0, 0, 0 };
			}
		}
	}
}
