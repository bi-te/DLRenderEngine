#pragma once

#include <vector>

#include "math.h"

class Mesh
{

public:
	std::vector<float> vertices_data_;
	uint8_t strip;

	void add_vertex(float x, float y, float z)
	{
		vertices_data_.push_back(x);
		vertices_data_.push_back(y);
		vertices_data_.push_back(z);

		vertices_data_.push_back(0.f);
		vertices_data_.push_back(0.f);
		vertices_data_.push_back(0.f);
	}

	void add_vertex(float x, float y, float z, 
					float r, float g, float b)
	{
		vertices_data_.push_back(x);
		vertices_data_.push_back(y);
		vertices_data_.push_back(z);

		vertices_data_.push_back(r);
		vertices_data_.push_back(g);
		vertices_data_.push_back(b);
	}
};
