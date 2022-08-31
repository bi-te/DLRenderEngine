#include "TextureManager.h"

TextureManager* TextureManager::s_manager;

comptr<ID3D11ShaderResourceView>& TextureManager::get(LPCWSTR texture)
{
	if (textures2d.find({ texture }) == textures2d.end())
		add_texture(texture);
	return textures2d.at({texture});
}

comptr<ID3D11ShaderResourceView> TextureManager::add_texture(LPCWSTR filename)
{
	if (textures2d.find({filename}) != textures2d.end()) return textures2d.at(filename);

	comptr<ID3D11ShaderResourceView> texture;
	CreateDDSTextureFromFileEx(Direct3D::instance().device5.Get(), filename, 0,
		D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, DXGI_CPU_ACCESS_NONE, 0, false,
		nullptr, &texture);
	textures2d.insert({ {filename}, texture });
	return texture;
}

comptr<ID3D11ShaderResourceView> TextureManager::add_cubemap(LPCWSTR filename)
{
	if (textures2d.find({ filename }) != textures2d.end()) return textures2d.at(filename);

	comptr<ID3D11ShaderResourceView> texture;
	CreateDDSTextureFromFileEx(Direct3D::instance().device5.Get(), filename, 0,
		D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, DXGI_CPU_ACCESS_NONE,
		D3D11_RESOURCE_MISC_TEXTURECUBE, false, nullptr, &texture);
	std::wstring file = filename;
	textures2d.insert({ file, texture });
	return texture;
}
