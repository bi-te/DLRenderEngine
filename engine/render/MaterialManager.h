#pragma once
#include <cassert>

#include "Material.h"
#include "TextureManager.h"

class MaterialManager
{
	static MaterialManager* s_manager;
	MaterialManager() = default;

	MaterialManager(const MaterialManager& other) = delete;
	MaterialManager(MaterialManager&& other) noexcept = delete;
	MaterialManager& operator=(const MaterialManager& other) = delete;
	MaterialManager& operator=(MaterialManager&& other) noexcept = delete;

	std::unordered_map<std::string, Material> materials;
public:

	static void init()
	{
		if (s_manager) reset();

		s_manager = new MaterialManager;
	}

	static MaterialManager& instance()
	{
		assert(s_manager && "MaterialManager not initialized");
		return *s_manager;
	}

	void add_material(const std::string& name, const std::vector<std::pair<TextureType, std::wstring>>& textures);
	void add(const std::string& name, const Material& material);

	Material& get(const std::string& name);

	static void reset()
	{
		delete s_manager;
	}
};

