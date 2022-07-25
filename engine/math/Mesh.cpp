#include "Mesh.h"

void Mesh::add_vertex(float x, float y, float z, float u, float v)
{
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);

	vertices.push_back(u);
	vertices.push_back(v);
}

void Mesh::add_vertex(const VertexTexture& vertex)
{
	vertices.push_back(vertex.coordinates.x);
	vertices.push_back(vertex.coordinates.y);
	vertices.push_back(vertex.coordinates.z);

	vertices.push_back(vertex.u);
	vertices.push_back(vertex.v);
}

Vertex Mesh::get_vertex_coor(uint32_t index)
{
	uint32_t pos = index * stride / sizeof(float);
	return Vertex{vertices[pos], vertices[pos + 1], vertices[pos + 2]};
}
