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


void OpaqueInstances::bind_instance_buffer()
{
	Direct3D& direct = Direct3D::instance();

	uint32_t instance_stride = sizeof(OpaqueInstanceRender), ioffset = 0;
	direct.context4->IASetVertexBuffers(1, 1, instanceBuffer.address(), &instance_stride, &ioffset);
	direct.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
							matrices[num_copied++].scale = model.model.get()->tree[mesh_node].mesh_matrix.topLeftCorner<3,3>() *
								transforms[model.instances[instance].model_world].normal_matrix;
						}
					}
					else {
						matrices[num_copied].model_transform = transforms[model.instances[instance].model_world].matrix();
						matrices[num_copied++].scale = transforms[model.instances[instance].model_world].normal_matrix;
					}
				}

	instanceBuffer.unmap();
}

void OpaqueInstances::render()
{
	Direct3D& direct = Direct3D::instance();

	opaqueShader->bind();
	update_instance_buffer();
	bind_instance_buffer();

	uint32_t renderedInstances = 0;
	for (const auto& per_model: perModels)
	{
		Model& model = *per_model.model;
		
		uint32_t stride = sizeof(AssimpVertex), pOffset = 0;
		direct.context4->IASetVertexBuffers(0, 1, model.vertexBuffer.address(), &stride, &pOffset);
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

			for (const auto& perMaterial: mesh.perMaterials)
			{
				const OpaqueMaterial& material = perMaterial.material;
				uint32_t instances = perMaterial.instances.size() * mesh.mesh_model_matrices.size();

				materialBuffer.write(&material.render_data);

				direct.context4->PSSetConstantBuffers(2, 1, materialBuffer.address());

				if(material.render_data.textures & MATERIAL_TEXTURE_DIFFUSE)
					direct.context4->PSSetShaderResources(5, 1, material.diffuse.GetAddressOf());

				if (material.render_data.textures & MATERIAL_TEXTURE_NORMAL)
					direct.context4->PSSetShaderResources(6, 1,	material.normals.GetAddressOf());

				if (material.render_data.textures & MATERIAL_TEXTURE_ROUGHNESS)
					direct.context4->PSSetShaderResources(7, 1, material.roughness.GetAddressOf());

				if (material.render_data.textures & MATERIAL_TEXTURE_METALLIC)
					direct.context4->PSSetShaderResources(8, 1, material.metallic.GetAddressOf());

				direct.context4->DrawIndexedInstanced(mrange.numIndices,
					instances, mrange.indicesOffset,
					mrange.verticesOffset, renderedInstances);
				renderedInstances += instances;
			}
		}
	}

}

void OpaqueInstances::shadow_render(uint32_t light_count)
{
	Direct3D& direct = Direct3D::instance();
	LightSystem& light_system = LightSystem::instance();

	update_instance_buffer();
	bind_instance_buffer();

	uint32_t renderedInstances = 0;
	for (const auto& per_model : perModels)
	{
		Model& model = *per_model.model;

		uint32_t stride = sizeof(AssimpVertex), pOffset = 0;
		direct.context4->IASetVertexBuffers(0, 1, model.vertexBuffer.address(), &stride, &pOffset);
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

			uint32_t instances = 0;
			for (const auto& perMaterial : mesh.perMaterials)
				instances += perMaterial.instances.size();
			instances *= mesh.mesh_model_matrices.size();

			for(uint32_t light = 0; light < light_count; light ++ )
			{
				light_system.bind_light_shadow_buffer(light);

				direct.context4->DrawIndexedInstanced(mrange.numIndices,
					instances, mrange.indicesOffset,
					mrange.verticesOffset, renderedInstances);
			}
			renderedInstances += instances;
		}
	}
}
