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
	uint32_t id;
};

class OpaqueInstances
{
public:
	struct Instance
	{
		ID model_world;
		std::vector<uint32_t> materials;
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
	std::shared_ptr<Shader> opaqueDeferredShader;
	std::shared_ptr<Shader> pointShadowShader;
	std::shared_ptr<Shader> spotShadowShader;
	std::vector<PerModel> perModels;

	void add_model_instance(const std::shared_ptr<Model>& model,
		const std::vector<OpaqueMaterial>& materials,
		const Instance& instance);
	void remove_model_instance(uint32_t modelInd, uint32_t instanceInd);

	void bind_instance_buffer();
	void update_instance_buffer();

	void render(bool forward_rendering);
	void shadow_render(uint32_t light_count);
};

