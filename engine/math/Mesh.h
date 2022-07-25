#pragma once

#include <vector>

#include "math.h"

struct Vertex
{
	float x, y, z;
};

struct VertexColor
{
	Vertex coordinates;
	float r, g, b;
};

struct VertexTexture
{
	Vertex coordinates;
	float u, v;
};

class Mesh
{

public:
	std::vector<float> vertices;
	std::vector<uint32_t> indices;
	uint32_t stride;
	
	void add_vertex(float x, float y, float z, float u, float v);
	void add_vertex(const VertexTexture& vertex);

	Vertex get_vertex_coor(uint32_t index);
};
