#include "TextureManager.h"

TextureManager* TextureManager::s_manager;

Texture& TextureManager::get(LPCWSTR filename)
{
	if (textures2d.find({ filename }) == textures2d.end())
		add_texture(filename);
	return *textures2d.at({ filename }).get();
}

std::shared_ptr<Texture> TextureManager::get_ptr(LPCWSTR filename) {
	if (textures2d.find({ filename }) == textures2d.end())
		add_texture(filename);
	return textures2d.at({ filename });
}

std::shared_ptr<Texture> TextureManager::add_texture(LPCWSTR filename)
{
	if (textures2d.find({ filename }) != textures2d.end()) return textures2d.at(filename);

	std::shared_ptr<Texture> texture{ new Texture };
	CreateDDSTextureFromFileEx(Direct3D::instance().device5.Get(), filename, 0,
		D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, DXGI_CPU_ACCESS_NONE, 0, false,
		nullptr, &texture->srv);
	textures2d.insert({ {filename}, texture });
	return texture;
}

std::shared_ptr<Texture> TextureManager::add_cubemap(LPCWSTR filename)
{
	if (textures2d.find({ filename }) != textures2d.end()) return textures2d.at(filename);

	std::shared_ptr<Texture> texture{ new Texture };
	CreateDDSTextureFromFileEx(Direct3D::instance().device5.Get(), filename, 0,
		D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, DXGI_CPU_ACCESS_NONE,
		D3D11_RESOURCE_MISC_TEXTURECUBE, false, nullptr, &texture->srv);
	textures2d.insert({ {filename}, texture });
	return texture;
}
