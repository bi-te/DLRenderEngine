#pragma once

#include <vector>

class Mesh
{
	std::vector<float> vertices_;

public:

	void add_vertex(float x, float y, float z)
	{
		vertices_.push_back(x);
		vertices_.push_back(y);
		vertices_.push_back(z);
	}
	

	const std::vector<float>& vertices()
	{
		return vertices_;
	}
	

};
