#pragma once
#include <vector>

#include "Screen.h"
#include "math/Sphere.h"
#include "math/Ray.h"

class Scene
{
public:

	std::vector<Sphere> objects;

	bool ray_collision(const Ray& ray, float t_min, hit_record& record) const;

	void draw(Screen& screen);
};
