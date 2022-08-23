#pragma once

#include <unordered_map>

#include "wchar_algorithms.h"
#include "Direct11/Direct3D.h"
#include "Direct11/DDSTextureLoader11.h"

enum TextureType{TextureDiffuse, TextureNormals, TextureMetallic, TextureRoughness};

class TextureManager
{
	static TextureManager* s_manager;
	TextureManager() = default;

	TextureManager(const TextureManager& other) = delete;
	TextureManager(TextureManager&& other) noexcept = delete;
	TextureManager& operator=(const TextureManager& other) = delete;
	TextureManager& operator=(TextureManager&& other) noexcept = delete;

	std::unordered_map<std::wstring, comptr<ID3D11ShaderResourceView>> textures2d;
public:

	static void init()
	{
		if (s_manager) reset();

		s_manager = new TextureManager;
	}

	static TextureManager& instance()
	{
		assert(s_manager && "TextureManager not initialized");
		return *s_manager;
	}

	static void reset()
	{
		delete s_manager;
	}

	const comptr<ID3D11ShaderResourceView>& get_texture(LPCWSTR filename);
	const comptr<ID3D11ShaderResourceView>& get_cubemap(LPCWSTR filename);

	void add_texture(LPCWSTR filename);
	void add_cubemap(LPCWSTR filename);
};

