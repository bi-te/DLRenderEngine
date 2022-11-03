#pragma once

#include <unordered_map>

#include "wchar_algorithms.h"
#include "Direct11/Direct3D.h"
#include "Direct11/DDSTextureLoader11.h"
#include "Direct11/Texture.h"

class TextureManager
{
	static TextureManager* s_manager;
	TextureManager() = default;

	TextureManager(const TextureManager& other) = delete;
	TextureManager(TextureManager&& other) noexcept = delete;
	TextureManager& operator=(const TextureManager& other) = delete;
	TextureManager& operator=(TextureManager&& other) noexcept = delete;

	std::unordered_map<std::wstring, std::shared_ptr<Texture>> textures2d;
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
		for (auto texture : s_manager->textures2d)
			texture.second.reset();
		delete s_manager;
	}

	Texture& get(LPCWSTR filename);
	std::shared_ptr<Texture> get_ptr(LPCWSTR filename);

	std::shared_ptr<Texture> add_texture(LPCWSTR filename);
	std::shared_ptr<Texture> add_cubemap(LPCWSTR filename);
};

