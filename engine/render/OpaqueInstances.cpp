#include "OpaqueInstances.h"

#include "LightSystem.h"
#include "ModelManager.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Material.h"
#include "moving/TransformSystem.h"

void OpaqueInstances::add_model_instance(const std::shared_ptr<Model>& model,
                                         const std::vector<OpaqueMaterial>& materials,
                                         const Instance& instance)
{
	assert(model->meshes.size() == materials.size() && "Number of Meshes and materials not equal");

	for (auto & perModel : perModels)
	{
		if(perModel.model.get() == model.get())
		{
			uint32_t new_instance = perModel.instances.size();
			perModel.instances.push_back(instance);

			for (uint32_t mesh_ind = 0; mesh_ind < perModel.perMeshes.size(); ++mesh_ind)
			{
				PerMesh& per_mesh = perModel.perMeshes.at(mesh_ind);
				bool new_material = true;
				for (uint32_t mat_ind = 0; mat_ind < per_mesh.perMaterials.size() && new_material; ++mat_ind)
				{
					PerMaterial& per_material = per_mesh.perMaterials.at(mat_ind);
					if (per_material.material.name == materials.at(mesh_ind).name)
					{
						per_material.instances.push_back(new_instance);
						new_material = false;
					}
				}
				if (new_material)
					per_mesh.perMaterials.push_back({
							materials.at(mesh_ind),{new_instance}
						});					
			}

			return;
		} 
	}

	PerModel perModel;
	perModel.model = model;
	perModel.perMeshes.resize(model->meshes.size());
	perModel.instances.push_back(instance);

	for (int ind = 0; ind < model->meshes.size(); ++ind)
	{
		perModel.perMeshes.at(ind).perMaterials.push_back({ materials[ind], {0u} });
		perModel.perMeshes[ind].mesh_model_matrices = model->meshes[ind].mesh_matrices;
	}

	perModels.push_back(std::move(perModel));
}

void OpaqueInstances::update_instance_buffer()
{
	solid_vector<Transform>& transforms = TransformSystem::instance().transforms;
	uint32_t num_instances = 0, num_copied = 0;

	for (auto& model : perModels)
		for (auto& mesh : model.perMeshes)
			for (auto& material : mesh.perMaterials)
				num_instances += material.instances.size() * mesh.mesh_model_matrices.size();
					

	if (!num_instances) return;

	instanceBuffer.allocate(num_instances * sizeof(OpaqueInstanceRender));

	OpaqueInstanceRender* matrices = static_cast<OpaqueInstanceRender*>(instanceBuffer.map().pData);

	for (auto& model : perModels)
		for (auto& mesh : model.perMeshes)
			for (auto& material : mesh.perMaterials)
				for (auto& instance : material.instances)
				{
					if (mesh.mesh_model_matrices.size() > 1) {
						for (auto& mesh_node : mesh.mesh_model_matrices)
						{
							matrices[num_copied].model_transform = model.model.get()->tree[mesh_node].mesh_matrix *
								transforms[model.instances[instance].model_world].matrix();
							matrices[num_copied++].scale = transforms[model.instances[instance].model_world].scale();
						}
					}
					else {
						matrices[num_copied].model_transform = transforms[model.instances[instance].model_world].matrix();
						matrices[num_copied++].scale = transforms[model.instances[instance].model_world].scale();
					}											
				}

	instanceBuffer.unmap();
}

void OpaqueInstances::render()
{
	Direct3D& direct = Direct3D::instance();

	const Shader& shader = ShaderManager::instance().operator()(opaqueShader.c_str());
	direct.context4->VSSetShader(shader.vertexShader.Get(), nullptr, NULL);
	direct.context4->PSSetShader(shader.pixelShader.Get(), nullptr, NULL);
	direct.context4->IASetInputLayout(shader.inputLayout.ptr.Get());

	update_instance_buffer();
	uint32_t instance_stride = sizeof(OpaqueInstanceRender), ioffset = 0;
	direct.context4->IASetVertexBuffers(1, 1, instanceBuffer.address(), &instance_stride, &ioffset);
	
	uint32_t renderedInstances = 0;
	for (const auto& per_model: perModels)
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
			direct.context4->VSSetConstantBuffers(2, 1, meshModel.address());

			for (const auto& perMaterial: mesh.perMaterials)
			{
				const OpaqueMaterial& material = perMaterial.material;
				uint32_t instances = perMaterial.instances.size() * mesh.mesh_model_matrices.size();

				materialBuffer.write(&material.render_data);

				direct.context4->PSSetConstantBuffers(3, 1, materialBuffer.address());

				if(material.render_data.hasDiffuseTexture)
					direct.context4->PSSetShaderResources(0, 1, 
						TextureManager::instance().get_texture(material.diffuse.c_str()).GetAddressOf());

				if (material.render_data.hasNormalsTexture)
					direct.context4->PSSetShaderResources(1, 1,	
						TextureManager::instance().get_texture(material.normals.c_str()).GetAddressOf());

				if (material.render_data.hasRoughnessTexture)
					direct.context4->PSSetShaderResources(2, 1, 
						TextureManager::instance().get_texture(material.roughness.c_str()).GetAddressOf());

				if (material.render_data.hasMetallicTexture)
					direct.context4->PSSetShaderResources(3, 1, 
						TextureManager::instance().get_texture(material.metallic.c_str()).GetAddressOf());

				Direct3D::instance().context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				direct.context4->DrawIndexedInstanced(mrange.numIndices,
					instances, mrange.indicesOffset,
					mrange.verticesOffset, renderedInstances);
				renderedInstances += instances;
			}
		}
	}

}
