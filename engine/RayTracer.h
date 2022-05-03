#pragma once

#include "Screen.h"
#include "RenderList.h"

class RayTracer
{
public:
	static void process_rays(const RenderObject& object, Screen& screen)
	{
		Ray ray;
		ray.set_direction({ 0, 0, 1 });

		hit_record record;

		for (int i = 0; i < screen.width; ++i)
		{
			for (int k = 0; k < screen.height; ++k)
			{
				ray.set_origin({ double(i),double(k), 0 });
				if (object.ray_collision(ray, 0, record))
				{
					screen.buffer[k * screen.width + i] = { 255, 255, 255 };
				}
				else
				{
					screen.buffer[k * screen.width + i] = { 0, 0, 0 };
				}
			}
		}

	}
};