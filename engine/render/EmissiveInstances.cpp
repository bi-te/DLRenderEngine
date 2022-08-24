#include "EmissiveInstances.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "moving/TransformSystem.h"

void EmissiveInstances::add_model_instance(const std::shared_ptr<Model>& model, const Instance& instance)
{
	for (auto& perModel : perModels)
	{
		if (perModel.model.get() == model.get())
		{
			perModel.instances.push_back(instance);
			return;
		}
	}

	PerModel perModel;
	perModel.model = model;
	perModel.perMeshes.resize(model->meshes.size());
	perModel.instances.push_back(instance);

	for (uint32_t ind = 0; ind < model->meshes.size(); ++ind)
		perModel.perMeshes[ind].mesh_model_matrices = model->meshes[ind].mesh_matrices;

	perModels.push_back(std::move(perModel));
}

void EmissiveInstances::update_instance_buffer()
{
	solid_vector<Transform>& transforms = TransformSystem::instance().transforms;
	uint32_t num_instances = 0, num_copied = 0;

	for (auto& model : perModels)
		for (auto& mesh : model.perMeshes)
			num_instances += model.instances.size() * mesh.mesh_model_matrices.size();

	if (!num_instances) return;

	instanceBuffer.allocate(num_instances * sizeof(EmissiveInstanceBuffer));

	EmissiveInstanceBuffer* emissive_instances = static_cast<EmissiveInstanceBuffer*>(instanceBuffer.map().pData);

	for (auto& model : perModels)
		for (auto& mesh : model.perMeshes)
				for (auto& instance : model.instances)
				{
					if (mesh.mesh_model_matrices.size() > 1)
						for (auto& mesh_node : mesh.mesh_model_matrices)
							emissive_instances[num_copied].model_world = model.model.get()->tree[mesh_node].mesh_matrix *
							transforms[instance.model_world].matrix();
					else
						emissive_instances[num_copied].model_world = transforms[instance.model_world].matrix();
					emissive_instances[num_copied++].emissive_light = instance.emissive_light;
				}

	instanceBuffer.unmap();
}

void EmissiveInstances::render()
{
	Direct3D& direct = Direct3D::instance();

	const Shader& shader = ShaderManager::instance().operator()(emissiceShader.c_str());
	direct.context4->VSSetShader(shader.vertexShader.Get(), nullptr, NULL);
	direct.context4->PSSetShader(shader.pixelShader.Get(), nullptr, NULL);
	direct.context4->IASetInputLayout(shader.inputLayout.ptr.Get());

	update_instance_buffer();
	uint32_t instance_stride = sizeof(EmissiveInstanceBuffer), ioffset = 0;
	direct.context4->IASetVertexBuffers(1, 1, instanceBuffer.address(), &instance_stride, &ioffset);
	direct.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	uint32_t renderedInstances = 0;
	for (const auto& per_model : perModels)
	{
		Model& model = *per_model.model;

		uint32_t stride = sizeof(AssimpVertex);
		direct.context4->IASetVertexBuffers(0, 1, model.vertexBuffer.address(), &stride, &ioffset);
		direct.context4->IASetIndexBuffer(model.indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);

		for (uint32_t mesh_ind = 0; mesh_ind < per_model.perMeshes.size(); ++mesh_ind)
		{
			const PerMesh& mesh = per_model.perMeshes[mesh_ind];
			Mesh::Range& mrange = model.meshes[mesh_ind].m_range;

			mat4f* matrices = (mat4f*)meshModel.map().pData;
			if (mesh.mesh_model_matrices.size() > 1)
				*matrices = mat4f::Identity();
			else
				*matrices = model.tree[mesh.mesh_model_matrices.at(0)].mesh_matrix;
			meshModel.unmap();
			direct.context4->VSSetConstantBuffers(1, 1, meshModel.address());

			uint32_t instances = per_model.instances.size() * mesh.mesh_model_matrices.size();

			direct.context4->DrawIndexedInstanced(mrange.numIndices,
				instances, mrange.indicesOffset,
				mrange.verticesOffset, renderedInstances);
			renderedInstances += instances;
		}
	}
}
