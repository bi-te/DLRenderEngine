#pragma once
#include <string>
#include <vector>

#include "Direct11/DynamicBuffer.h"
#include "math/math.h"
#include "Material.h"
#include "Direct11/Direct3D.h"
#include "data_structures/solid_vector.h"
#include "objects/Model.h"

struct Shader;

struct OpaqueInstanceRender
{
	mat4f model_transform;
	mat3f scale;
};

class OpaqueInstances
{
public:
	struct Instance
	{
		ID model_world;
	};

private:
	struct PerMaterial
	{
		OpaqueMaterial material;
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
		materialBuffer.allocate(sizeof(BufferMaterial));
	}

	DynamicBuffer instanceBuffer{ D3D11_BIND_VERTEX_BUFFER };
	DynamicBuffer meshModel{ D3D11_BIND_CONSTANT_BUFFER };
	DynamicBuffer materialBuffer{ D3D11_BIND_CONSTANT_BUFFER };

	std::shared_ptr<Shader> opaqueShader;
	std::vector<PerModel> perModels;

	void add_model_instance(const std::shared_ptr<Model>& model,
	                        const std::vector<OpaqueMaterial>& materials,
	                        const Instance& instance);

	void bind_instance_buffer();
	void update_instance_buffer();

	void render();
	void mesh_render(uint32_t render_count);
};

