#pragma once
#include "Material.h"
#include "objects/Model.h"
#include "data_structures/solid_vector.h"
#include "Direct11/DynamicBuffer.h"
#include "math/math.h"

struct EmissiveInstanceBuffer
{
	mat4f model_world;

	vec3f emissive_light;
	float padding0;
};

class EmissiveInstances
{
public:
	struct Instance
	{
		vec3f emissive_light;
		ID model_world;
	};

private:
	struct PerMesh
	{
		std::vector<uint32_t> mesh_model_matrices;
	};

	struct PerModel
	{
		std::shared_ptr<Model> model;
		std::vector<Instance> instances;
		std::vector<PerMesh> perMeshes;
	};
public:
	explicit EmissiveInstances()
	{
		meshModel.allocate(sizeof(mat4f));
	}

	DynamicBuffer instanceBuffer{ D3D11_BIND_VERTEX_BUFFER };
	DynamicBuffer meshModel{ D3D11_BIND_CONSTANT_BUFFER };
	std::wstring emissiceShader;
	std::vector<PerModel> perModels;

	void add_model_instance(const std::shared_ptr<Model>& model, const Instance& instance);

	void update_instance_buffer();
	void render();
};

