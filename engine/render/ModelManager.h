#pragma once

#include <unordered_map>

#include "Material.h"
#include "TextureManager.h"

#include "assimp/scene.h"
#include "objects/Model.h"
#include "render/OpaqueInstances.h"


class ModelManager
{
	static ModelManager* s_manager;
	ModelManager() = default;

	ModelManager(const ModelManager& other) = delete;
	ModelManager(ModelManager&& other) noexcept = delete;
	ModelManager& operator=(const ModelManager& other) = delete;
	ModelManager& operator=(ModelManager&& other) noexcept = delete;

	std::unordered_map<std::string, std::shared_ptr<Model>> models;

	void parse_tree(const aiScene& scene, Model& model);
	OpaqueMaterial load_material(aiMaterial& material, std::string& dir);
public:

	static void init()
	{
		if (s_manager) reset();

		s_manager = new ModelManager;
	}

	static ModelManager& instance()
	{
		assert(s_manager && "ModelManager not initialized");
		return *s_manager;
	}

	static void reset()
	{
		delete s_manager;
	}

	void init_cube();
	void init_quad();
	void init_flat_cube_sphere(uint32_t grid_size);
	void init_sphere(uint32_t sectors, uint32_t stacks);
	void init_flat_sphere(uint32_t sectors, uint32_t stacks);

	void add_model(const std::string& path);
	Model& get_model(const std::string& model);
	std::shared_ptr<Model> get_ptr(const std::string& model);
};

