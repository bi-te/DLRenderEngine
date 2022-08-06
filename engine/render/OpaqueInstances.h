#pragma once
#include <string>
#include <vector>

#include "Direct11/DynamicBuffer.h"
#include "math/math.h"
#include "Material.h"
#include "math/Transform.h"

class Model;
class ModelManager;
struct Material;

struct Instance
{
	std::vector<mat4f> mesh_model;
	Transform model_world;
};

class OpaqueInstances
{
	friend ModelManager;

	struct PerMaterial
	{
		Material material;
		std::vector<uint32_t> instances;
	};

	struct PerMesh
	{
		std::vector<uint32_t> mesh_model_matrices;
		std::vector<PerMaterial> perMaterials;
	};

	struct PerModel
	{
		std::shared_ptr<Model> model;
		std::vector<Instance> instances;
		std::vector<PerMesh> perMeshes;
	};

public:

	DynamicBuffer<D3D11_BIND_VERTEX_BUFFER> instanceBuffer;
	std::vector<PerModel> perModels;

	Instance* add_model_instance(const std::shared_ptr<Model>& model, const std::vector<Material>& materials);

	void update_instance_buffer();
	void render();
};

