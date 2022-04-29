#pragma once

#include <vector>

#include "sphere.h"

struct rgb
{
	uint8_t b, g, r;
};

struct screen
{
	uint16_t width, height;
	std::vector<rgb> buffer;

	screen(uint16_t width = 800, uint16_t height = 600): width(width), height(height)
	{
		buffer.resize(width * height);
	}

	void set_size(int16_t w, int16_t h)
	{
		width = w;
		height = h;
		buffer.resize(width * height);
	}
};

struct mouse
{
	int mx, my;
	bool can_move = false;
};



class engine
{
public:
	
	sphere r_sphere;
	screen screen;
	mouse mouse;

	void process_rays()
	{
		ray ray;
		ray.set_direction({ 0, 0, 1 });

		for (int i = 0; i < screen.width; ++i)
		{
			for (int k = 0; k < screen.height; ++k)
			{
				ray.set_origin({ i,k, 0 });
				if (check_collision(ray, r_sphere))
				{
					screen.buffer[k * screen.width + i] = {255, 255, 255};
				}
				else
				{
					screen.buffer[k * screen.width + i] = {0, 0, 0};
				}
			}
		}

	}

	static bool check_collision(const ray& ray, const sphere& sphere)
	{
		vec3 oc = ray.get_origin() - sphere.get_center();
		double a = ray.get_direction() * ray.get_direction();
		double b = 2 * (ray.get_direction() * oc);
		double c = oc * oc - pow(sphere.get_radius(), 2);
		return b * b - 4 * a * c > 0;
	}

	static engine& instance()
	{
		static std::unique_ptr<engine> engine_(new engine);
		return  *engine_;
	}

private:
	engine()
	{
		r_sphere = { { screen.width / 2, screen.height / 2, 0 }, 50 };
	}
};