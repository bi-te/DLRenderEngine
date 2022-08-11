#include "MaterialManager.h"

MaterialManager* MaterialManager::s_manager;

void MaterialManager::add_material(const std::string& name,
	const std::vector<std::pair<TextureType, std::wstring>>& textures)
{
	if (materials.count(name)) return;
	
	Material mat;
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
	materials.insert({ name, std::move(mat) });
}

void MaterialManager::add(const std::string& name, const Material& material)
{
	if (materials.count(name)) return;
	materials.insert({ name, material });
}

Material& MaterialManager::get(const std::string& name)
{
	assert(materials.count(name));
	return materials.at(name);
}
