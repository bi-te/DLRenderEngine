#pragma once

#include <unordered_map>

#include "wchar_algorithms.h"
#include "Direct11/Direct3D.h"
#include "Direct11/DDSTextureLoader11.h"

class TextureManager
{
	std::unordered_map<LPCWSTR, comptr<ID3D11ShaderResourceView>, pwchar_hash, pwchar_comparator> textures2d;

	static TextureManager* s_manager;
	TextureManager()
	{
	}
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

