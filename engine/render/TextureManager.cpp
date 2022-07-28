#include "TextureManager.h"

TextureManager* TextureManager::s_manager;

const comptr<ID3D11ShaderResourceView>& TextureManager::get_texture(LPCWSTR texture)
{
	if (textures2d.count(texture))
		add_texture(texture);
	return textures2d.at(texture);
}

const comptr<ID3D11ShaderResourceView>& TextureManager::get_cubemap(LPCWSTR texture)
{
	if (!textures2d.count(texture))
		add_cubemap(texture);
	return textures2d.at(texture);
}

void TextureManager::add_texture(LPCWSTR filename)
{
	comptr<ID3D11ShaderResourceView> texture;
	CreateDDSTextureFromFileEx(Direct3D::instance().device5.Get(), filename, 0,
		D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, DXGI_CPU_ACCESS_NONE, 0, false,
		nullptr, &texture);
	textures2d.insert({ filename, std::move(texture) });
	
}

void TextureManager::add_cubemap(LPCWSTR filename)
{
	comptr<ID3D11ShaderResourceView> texture;
	CreateDDSTextureFromFileEx(Direct3D::instance().device5.Get(), filename, 0,
		D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, DXGI_CPU_ACCESS_NONE,
		D3D11_RESOURCE_MISC_TEXTURECUBE, false, nullptr, &texture);
	std::wstring file = filename;
	textures2d.insert({ filename, std::move(texture) });
	
}
