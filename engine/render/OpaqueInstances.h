#pragma once
#include <string>
#include <vector>

#include "Direct11/DynamicBuffer.h"
#include "math/math.h"
#include "Material.h"
#include "Direct11/Direct3D.h"
#include "math/Transform.h"

class Model;
class ModelManager;
struct Material;

struct Instance
{
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
	explicit OpaqueInstances()
	{
		meshModel.allocate(sizeof(mat4f));
	}

	DynamicBuffer instanceBuffer{ D3D11_BIND_VERTEX_BUFFER };
	DynamicBuffer meshModel{ D3D11_BIND_CONSTANT_BUFFER };
	std::wstring opaqueShader;
	std::vector<PerModel> perModels;

	void add_model_instance(const std::shared_ptr<Model>& model,
	                        const std::vector<Material>& materials,
	                        const Instance& instance);

	void update_instance_buffer();
	void render();
};

