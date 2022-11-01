#pragma once

#include <vector>

#include "Direct11/DynamicBuffer.h"
#include "Direct11/Texture.h"
#include "math/math.h"
#include "Material.h"
#include "data_structures/solid_vector.h"
#include "objects/Model.h"

struct Shader;

struct AppearingInstanceBuffer
{
	mat4f model_transform;
	mat3f scale;
	vec3f appearanceColor;
	float animationFraction;
};

class AppearingInstances
{
public:
	struct Instance
	{
		ID model_world;
		vec3f appearanceColor;
		float animationTime;
		float animationPassed = 0.f;
		std::vector<uint32_t> materials;

		Instance(ID model_world, vec3f appearanceColor, float animationTime)
		:model_world(model_world), appearanceColor(appearanceColor), animationTime(animationTime){}
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
	explicit AppearingInstances()
	{
		meshModel.allocate(sizeof(mat4f));
		materialBuffer.allocate(sizeof(BufferMaterial));
	}

	DynamicBuffer instanceBuffer{ D3D11_BIND_VERTEX_BUFFER };
	DynamicBuffer meshModel{ D3D11_BIND_CONSTANT_BUFFER };
	DynamicBuffer materialBuffer{ D3D11_BIND_CONSTANT_BUFFER };

	std::shared_ptr<Texture> noiseTexture;
	std::shared_ptr<Shader> appearShader;
	std::shared_ptr<Shader> appearDeferredShader;
	std::shared_ptr<Shader> pointShadowShader;
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

