#pragma once

#pragma once

#include "data_structures/TriangleOctree.h"
#include "math/AABB.h"
#include "math/math.h"
#include "render/Direct11/ImmutableBuffer.h"

struct Model;

struct AssimpVertex
{
	vec3f coor;
	vec2f texcoor;
	vec3f normal;
	vec3f tangent;
	vec3f bitangent;
};

struct Mesh
{
	struct Range
	{
		uint32_t numVertices, numIndices;
		uint32_t verticesOffset, indicesOffset;
	};

	const Model* model;
	std::vector<uint32_t> mesh_matrices;

	Range m_range;
	BoundingBox box;
};

struct Model
{
	struct Node
	{
		mat4f mesh_matrix, mesh_matrix_inv;
	};

	std::string name;
	std::vector<Node> tree;
	std::vector<Mesh> meshes;

	std::vector<uint32_t> indices;
	std::vector<AssimpVertex> vertices;

	std::vector<TriangleOctree> octrees;
	BoundingBox bbox;
	ImmutableBuffer vertexBuffer{ D3D11_BIND_VERTEX_BUFFER };
	ImmutableBuffer indexBuffer{ D3D11_BIND_INDEX_BUFFER };
};