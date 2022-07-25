#pragma once

#include <vector>

#include "Direct11/Direct3D.h"
#include "Direct11/DDSTextureLoader11.h"

struct Texture2D
{
	comptr<ID3D11ShaderResourceView> srvTexture;
	comptr<ID3D11Resource> texture;
};

class TextureManager
{
	std::vector<Texture2D> textures2d;

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

	const Texture2D& operator [](uint32_t ind) { return textures2d[ind]; }

	uint32_t add_texture(LPCWSTR filename);
	uint32_t add_cubemap(LPCWSTR filename);
};

