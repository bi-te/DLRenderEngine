#pragma once
#include <unordered_map>

#include "render/OpaqueInstances.h"
#include "assimp/scene.h"
#include "math/math.h"
#include "Direct11/ImmutableBuffer.h"
#include "Material.h"
#include "TextureManager.h"

struct AssimpVertex
{
	vec3f coor;
	vec2f texcoor;
	vec3f normal;
	vec3f tangent;
	vec3f bitangent;
};

struct Model
{
	struct MeshRange
	{
		uint32_t numIndices, numVertices;
		uint32_t indicesOffset, verticesOffset;
	};

	struct Node
	{
		mat4f mesh_matrix;
		std::vector<uint32_t> meshes;
	};
	
	std::string name;
	std::vector<Node> tree;
	std::vector<MeshRange> meshes;
	ImmutableBuffer<D3D11_BIND_VERTEX_BUFFER> vertexBuffer;
	ImmutableBuffer<D3D11_BIND_INDEX_BUFFER> indexBuffer;
};

class ModelManager
{
	static ModelManager* s_manager;
	ModelManager(){}

	std::unordered_map<std::string, std::shared_ptr<Model>> models;

	void parse_tree(const aiScene& scene, Model& model);
	Material load_material(aiMaterial& material, std::string& path);
	std::wstring load_texture(aiTextureType type, aiMaterial& material, const std::string& directory);
public:

	static void init()
	{
		if (s_manager) reset();

		s_manager = new ModelManager;
	}

	static ModelManager& instance()
	{
		assert(s_manager && "TextureManager not initialized");
		return *s_manager;
	}

	static void reset()
	{
		delete s_manager;
	}

	void make_cube();

	void add_model(const std::string& path);
	Model& get_model(const std::string& model);
	std::shared_ptr<Model> get_ptr(const std::string& model);
};

