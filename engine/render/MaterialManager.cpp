#include "MaterialManager.h"

MaterialManager* MaterialManager::s_manager;

void MaterialManager::add(const OpaqueMaterial& material)
{
	if (op_materials.find(material.name) != op_materials.end()) return;

	op_materials.insert({ material.name, material });
}

void MaterialManager::add_opaque_material(const std::string& name,
	const std::vector<std::pair<TextureType, std::shared_ptr<Texture>>>& textures)
{
	if (op_materials.find(name) != op_materials.end()) return;
	
	OpaqueMaterial mat;
	mat.name = name;
	for (const auto & texture : textures)
	{
		switch (texture.first)
		{
		case (TextureDiffuse): mat.diffuse = texture.second; break;
		case (TextureNormals): mat.normals = texture.second; break;
		case (TextureMetallic): mat.metallic = texture.second; break;
		case (TextureRoughness): mat.roughness = texture.second; break;
		}
	}
	op_materials.insert({ name, std::move(mat) });
}

OpaqueMaterial& MaterialManager::get_opaque(const std::string& name)
{
	assert(op_materials.find(name) != op_materials.end());
	return op_materials.at(name);
}
