#include "MaterialManager.h"

MaterialManager* MaterialManager::s_manager;

void MaterialManager::add(const OpaqueMaterial& material)
{
	if (op_materials.count(material.name)) return;

	if(material.render_data.hasDiffuseTexture)
		TextureManager::instance().add_texture(material.diffuse.c_str());
	if (material.render_data.hasNormalsTexture)
		TextureManager::instance().add_texture(material.normals.c_str());
	if (material.render_data.hasRoughnessTexture)
		TextureManager::instance().add_texture(material.roughness.c_str());
	if (material.render_data.hasMetallicTexture)
		TextureManager::instance().add_texture(material.metallic.c_str());

	op_materials.insert({ material.name, material });
}

void MaterialManager::add_opaque_material(const std::string& name,
	const std::vector<std::pair<TextureType, std::wstring>>& textures)
{
	if (op_materials.count(name)) return;
	
	OpaqueMaterial mat;
	mat.name = name;
	for (const auto & texture : textures)
	{
		switch (texture.first)
		{
		case (TextureDiffuse): mat.diffuse = texture.second;
		case (TextureNormals): mat.normals = texture.second;
		case (TextureMetallic): mat.metallic = texture.second;
		case (TextureRoughness): mat.roughness = texture.second;
		}
	}
	op_materials.insert({ name, std::move(mat) });
}

OpaqueMaterial& MaterialManager::get_opaque(const std::string& name)
{
	assert(op_materials.count(name));
	return op_materials.at(name);
}
