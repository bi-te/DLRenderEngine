#include "TextureManager.h"

TextureManager* TextureManager::s_manager;

uint32_t TextureManager::add_texture(LPCWSTR filename)
{
	Texture2D texture;
	CreateDDSTextureFromFileEx(Direct3D::globals().device5.Get(), filename, 0,
		D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, DXGI_CPU_ACCESS_NONE, 0, false,
		&texture.texture, &texture.srvTexture);
	textures2d.push_back(std::move(texture));

	return textures2d.size() - 1u;
}

uint32_t TextureManager::add_cubemap(LPCWSTR filename)
{
	Texture2D texture;
	CreateDDSTextureFromFileEx(Direct3D::globals().device5.Get(), filename, 0,
		D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, DXGI_CPU_ACCESS_NONE,
		D3D11_RESOURCE_MISC_TEXTURECUBE, false, &texture.texture, &texture.srvTexture);
	textures2d.push_back(std::move(texture));

	return textures2d.size() - 1u;
}
