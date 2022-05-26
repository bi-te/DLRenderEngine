#pragma once

#include <vector>

#include "math.h"

class Mesh
{
protected:

	std::vector<vec3> vertices_;

public:

	void add_vertex(float x, float y, float z)
	{
		vertices_.push_back({ x, y, z });
	}

	void add_vertex(const vec3& coordinate)
	{
		vertices_.push_back(coordinate);
	}

	const std::vector<vec3>& vertices()
	{
		return vertices_;
	}
	

};
