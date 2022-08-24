#include "ModelManager.h"

#include <queue>

#include "MaterialManager.h"
#include "TextureManager.h"
#include "wchar_algorithms.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "Direct11/Direct3D.h"

ModelManager* ModelManager::s_manager;

void ModelManager::add_model(const std::string& path)
{
	if (models.count(path)) return;

	std::string dir = path.substr(0, path.find_last_of("\\/") + 1);
	
	std::shared_ptr<Model> s_model{ new Model };
	Model& model = *s_model;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenBoundingBoxes
		| aiProcess_CalcTangentSpace | aiProcess_ConvertToLeftHanded | aiProcess_JoinIdenticalVertices);

	model.name = path;
	model.meshes.resize(scene->mNumMeshes);
	model.octrees.resize(scene->mNumMeshes);

	uint32_t vsize = 0, isize = 0;
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		Mesh& m_mesh = model.meshes[i];
		aiMesh& mesh = *scene->mMeshes[i];

		m_mesh.model = &model;
		m_mesh.m_range.indicesOffset = isize;
		m_mesh.m_range.verticesOffset = vsize;
		
		for (int i = 0; i < mesh.mNumVertices; ++i, ++vsize)
		{
			AssimpVertex vertex;
			vertex.coor = reinterpret_cast<vec3f&>(mesh.mVertices[i]);
			vertex.texcoor = reinterpret_cast<vec2f&>(mesh.mTextureCoords[0][i]);
			vertex.normal = reinterpret_cast<vec3f&>(mesh.mNormals[i]);
			vertex.tangent = reinterpret_cast<vec3f&>(mesh.mTangents[i]);
			vertex.bitangent = reinterpret_cast<vec3f&>(mesh.mBitangents[i]);
			model.vertices.push_back(vertex);
		}

		for (int i = 0; i < mesh.mNumFaces; ++i, isize += 3)
		{
			aiFace& face = mesh.mFaces[i];
			model.indices.push_back(face.mIndices[0]);
			model.indices.push_back(face.mIndices[1]);
			model.indices.push_back(face.mIndices[2]);
		}

		m_mesh.m_range.numVertices = vsize - m_mesh.m_range.verticesOffset;
		m_mesh.m_range.numIndices = isize - m_mesh.m_range.indicesOffset;

		m_mesh.box = reinterpret_cast<BoundingBox&>(mesh.mAABB);
		model.octrees[i].initialize(m_mesh);
	}

	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		load_material(*scene->mMaterials[i], dir);
	}

	model.vertexBuffer.write(model.vertices.data(), vsize * sizeof(AssimpVertex));
	model.indexBuffer.write(model.indices.data(), isize * sizeof(uint32_t));

	parse_tree(*scene, model);

	models.insert({ path, s_model });
}

Model& ModelManager::get_model(const std::string& model)
{
	if (!models.count(model))
		add_model(model);

	return *models.at(model);
}

std::shared_ptr<Model> ModelManager::get_ptr(const std::string& model)
{
	if (!models.count(model))
		add_model(model);

	return models.at(model);
}

OpaqueMaterial ModelManager::load_material(aiMaterial& material, std::string& dir)
{
	aiString texture;
	std::string wtexture;
	OpaqueMaterial mat{};
	
	mat.name = dir + material.GetName().C_Str();

	if (material.GetTextureCount(aiTextureType_DIFFUSE))
	{
		material.GetTexture(aiTextureType_DIFFUSE, 0, &texture);
		wtexture = dir + texture.C_Str();
		mat.diffuse = char_to_wchar(wtexture.c_str());
		mat.render_data.textures |= MATERIAL_TEXTURE_DIFFUSE;
		TextureManager::instance().add_texture(mat.diffuse.c_str());
	}

	if (material.GetTextureCount(aiTextureType_NORMALS))
	{
		material.GetTexture(aiTextureType_NORMALS, 0, &texture);
		wtexture = dir + texture.C_Str();
		mat.normals = char_to_wchar(wtexture.c_str());
		mat.render_data.textures |= MATERIAL_TEXTURE_NORMAL;
		TextureManager::instance().add_texture(mat.normals.c_str());
	}

	if (material.GetTextureCount(aiTextureType_METALNESS))
	{
		material.GetTexture(aiTextureType_METALNESS, 0, &texture);
		wtexture = dir + texture.C_Str();
		mat.metallic = char_to_wchar(wtexture.c_str());
		mat.render_data.textures |= MATERIAL_TEXTURE_METALLIC;
		TextureManager::instance().add_texture(mat.metallic.c_str());
	}
	else mat.render_data.metallic = BASE_METALLIC;

	if (material.GetTextureCount(aiTextureType_SHININESS))
	{
		material.GetTexture(aiTextureType_SHININESS, 0, &texture);
		wtexture = dir + texture.C_Str();
		mat.roughness = char_to_wchar(wtexture.c_str());
		mat.render_data.textures |= MATERIAL_TEXTURE_ROUGHNESS;
		TextureManager::instance().add_texture(mat.roughness.c_str());
	}
	else mat.render_data.roughness = BASE_ROUGHNESS;
	
	MaterialManager::instance().add(std::move(mat));

	return mat;
}

void ModelManager::parse_tree(const aiScene& scene, Model& model)
{
	std::queue<aiNode*> nodes;
	Model::Node root_node;

	nodes.push(scene.mRootNode);
	
	root_node.mesh_matrix = mat4f::Identity();
	model.tree.push_back(root_node);

	for (uint32_t ind = 0, visited_nodes = 1; !nodes.empty(); ++ind)
	{
		aiNode& node = *nodes.front(); nodes.pop();
		model.tree.resize(model.tree.size() + node.mNumChildren);

		Model::Node& tree_node = model.tree.at(ind);

		tree_node.mesh_matrix *= reinterpret_cast<mat4f&>(node.mTransformation.Transpose());
		tree_node.mesh_matrix_inv = tree_node.mesh_matrix.inverse();

		for (int i = 0; i < node.mNumMeshes; ++i)
			model.meshes[node.mMeshes[i]].mesh_matrices.push_back(ind);
					
		for (uint32_t child_ind = 0; child_ind < node.mNumChildren; ++child_ind)
		{
			uint32_t child_node = visited_nodes + child_ind;
			model.tree.at(child_node).mesh_matrix = tree_node.mesh_matrix;

			nodes.push(node.mChildren[child_ind]);
		}

		visited_nodes += node.mNumChildren;
	}
}

void ModelManager::init_cube()
{
	if (models.count("Cube")) return;

	std::shared_ptr<Model> cube_model{ new Model };
	Model& cube = *cube_model.get();

	cube.name = "Cube";
	cube.tree = {
		{mat4f::Identity(), mat4f::Identity()}
	};

	cube.meshes = {
		{ &cube, {0}, {24, 36, 0, 0}, {{-0.5f, -0.5f, -0.5f}, {0.5f,  0.5f,  0.5f}} }
	};

	cube.vertices = {
	{{ 0.5f, -0.5f,  0.5f},   {0.f,  1.f}, { 0.f,  0.f,  1.f}, {-1.f, 0.f,  0.f}, {0.f, -1.f,  0.f}},
	{{ 0.5f,  0.5f,  0.5f},   {0.f,  0.f}, { 0.f,  0.f,  1.f}, {-1.f, 0.f,  0.f}, {0.f, -1.f,  0.f}},
	{{-0.5f,  0.5f,  0.5f},   {1.f,  0.f}, { 0.f,  0.f,  1.f}, {-1.f, 0.f,  0.f}, {0.f, -1.f,  0.f}},
	{{-0.5f, -0.5f,  0.5f},   {1.f,  1.f}, { 0.f,  0.f,  1.f}, {-1.f, 0.f,  0.f}, {0.f, -1.f,  0.f}},
	 
	{{-0.5f, -0.5f, -0.5f},   {0.f,  1.f}, { 0.f,  0.f, -1.f}, { 1.f, 0.f,  0.f}, {0.f, -1.f,  0.f}},
	{{-0.5f,  0.5f, -0.5f},   {0.f,  0.f}, { 0.f,  0.f, -1.f}, { 1.f, 0.f,  0.f}, {0.f, -1.f,  0.f}},
	{{ 0.5f,  0.5f, -0.5f},   {1.f,  0.f}, { 0.f,  0.f, -1.f}, { 1.f, 0.f,  0.f}, {0.f, -1.f,  0.f}},
	{{ 0.5f, -0.5f, -0.5f},   {1.f,  1.f}, { 0.f,  0.f, -1.f}, { 1.f, 0.f,  0.f}, {0.f, -1.f,  0.f}},

	{{ 0.5f, -0.5f, -0.5f},   {0.f,  1.f}, { 1.f,  0.f,  0.f}, { 0.f, 0.f,  1.f}, {0.f, -1.f,  0.f}},
	{{ 0.5f,  0.5f, -0.5f},   {0.f,  0.f}, { 1.f,  0.f,  0.f}, { 0.f, 0.f,  1.f}, {0.f, -1.f,  0.f}},
	{{ 0.5f,  0.5f,  0.5f},   {1.f,  0.f}, { 1.f,  0.f,  0.f}, { 0.f, 0.f,  1.f}, {0.f, -1.f,  0.f}},
	{{ 0.5f, -0.5f,  0.5f},   {1.f,  1.f}, { 1.f,  0.f,  0.f}, { 0.f, 0.f,  1.f}, {0.f, -1.f,  0.f}},

	{{-0.5f, -0.5f,  0.5f},   {0.f,  1.f}, {-1.f,  0.f,  0.f}, { 0.f, 0.f, -1.f}, {0.f, -1.f,  0.f}},
	{{-0.5f,  0.5f,  0.5f},   {0.f,  0.f}, {-1.f,  0.f,  0.f}, { 0.f, 0.f, -1.f}, {0.f, -1.f,  0.f}},
	{{-0.5f,  0.5f, -0.5f},   {1.f,  0.f}, {-1.f,  0.f,  0.f}, { 0.f, 0.f, -1.f}, {0.f, -1.f,  0.f}},
	{{-0.5f, -0.5f, -0.5f},   {1.f,  1.f}, {-1.f,  0.f,  0.f}, { 0.f, 0.f, -1.f}, {0.f, -1.f,  0.f}},

	{{-0.5f,  0.5f, -0.5f},   {0.f,  1.f}, { 0.f,  1.f,  0.f}, { 1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	{{-0.5f,  0.5f,  0.5f},   {0.f,  0.f}, { 0.f,  1.f,  0.f}, { 1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	{{ 0.5f,  0.5f,  0.5f},   {1.f,  0.f}, { 0.f,  1.f,  0.f}, { 1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	{{ 0.5f,  0.5f, -0.5f},   {1.f,  1.f}, { 0.f,  1.f,  0.f}, { 1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},

	{{ 0.5f, -0.5f, -0.5f},   {0.f,  1.f}, { 0.f, -1.f,  0.f}, {-1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	{{ 0.5f, -0.5f,  0.5f},   {0.f,  0.f}, { 0.f, -1.f,  0.f}, {-1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	{{-0.5f, -0.5f,  0.5f},   {1.f,  0.f}, { 0.f, -1.f,  0.f}, {-1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	{{-0.5f, -0.5f, -0.5f},   {1.f,  1.f}, { 0.f, -1.f,  0.f}, {-1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	};

	cube.indices = {
		0,  1,   2,  0,  2,  3,
		4,  5,   6,  4,  6,  7,
		8,  9,  10,  8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	cube.octrees.emplace_back();
	cube.octrees.back().initialize(cube.meshes.back());

	cube.vertexBuffer.write(cube.vertices.data(), cube.vertices.size() * sizeof(AssimpVertex));
	cube.indexBuffer.write(cube.indices.data(), cube.indices.size() * sizeof(uint32_t));

	models.insert({ "Cube", cube_model });
}

void ModelManager::init_quad()
{
	if (models.count("Quad")) return;

	std::shared_ptr<Model> quad_model{ new Model };
	Model& quad = *quad_model.get();

	quad.name = "Quad";
	quad.tree = {
		{mat4f::Identity(), mat4f::Identity()}
	};

	quad.meshes = {
		{ &quad, {0}, {4, 6, 0, 0}, {{-0.5f, -0.5f, -0.5f}, {0.5f,  0.5f,  0.5f}} }
	};

	quad.vertices = {
	{{-0.5f,  0.0f, -0.5f},   {0.f,  1.f}, { 0.f,  1.f,  0.f}, { 1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	{{-0.5f,  0.0f,  0.5f},   {0.f,  0.f}, { 0.f,  1.f,  0.f}, { 1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	{{ 0.5f,  0.0f,  0.5f},   {1.f,  0.f}, { 0.f,  1.f,  0.f}, { 1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	{{ 0.5f,  0.0f, -0.5f},   {1.f,  1.f}, { 0.f,  1.f,  0.f}, { 1.f, 0.f,  0.f}, {0.f,  0.f, -1.f}},
	};

	quad.indices = {
		0, 1, 2, 0, 2, 3,
	};

	quad.octrees.emplace_back();
	quad.octrees.back().initialize(quad.meshes.back());

	quad.vertexBuffer.write(quad.vertices.data(), quad.vertices.size() * sizeof(AssimpVertex));
	quad.indexBuffer.write(quad.indices.data(), quad.indices.size() * sizeof(uint32_t));

	models.insert({ "Quad", quad_model });
}

void ModelManager::init_flat_cube_sphere(uint32_t grid_size)
{
	const uint32_t SIDES = 6;
	const uint32_t TRIANGLES_PER_SIDE = grid_size * grid_size * 2;
	const uint32_t VERT_PER_SIDE = 3 * TRIANGLES_PER_SIDE;

	const char* model_name = "FlatCubeSphere";
	if (models.count(model_name)) return;

	uint32_t num_vertices = VERT_PER_SIDE * SIDES;
	uint32_t num_indices = num_vertices;

	std::shared_ptr<Model> cube_sphere_model{ new Model };
	Model& cube_sphere = *cube_sphere_model.get();

	cube_sphere.name = model_name;
	cube_sphere.tree = {
		{mat4f::Identity(), mat4f::Identity()}
	};

	cube_sphere.meshes = {
		{ &cube_sphere, {0}, {num_vertices, num_indices, 0, 0}, {{-1.f, -1.f, -1.f}, {1.f,  1.f,  1.f}} }
	};

	cube_sphere.vertices.resize(num_vertices);
	cube_sphere.indices.resize(num_indices);

	AssimpVertex* vertex = cube_sphere.vertices.data();

	int sideMasks[6][3] =
	{
		{ 0, 1, 2 },
		{ 2, 1, 0 },
		{ 0, 1, 2 },
		{ 2, 1, 0 },
		{ 2, 0, 1 },
		{ 2, 0, 1 }
	};

	float sideSigns[6][3] =
	{
		{ +1, +1, +1 },
		{ -1, +1, +1 },
		{ -1, +1, -1 },
		{ +1, +1, -1 },
		{ +1, -1, -1 },
		{ +1, +1, +1 }
	};

	
	for (int side = 0; side < SIDES; ++side)
	{
		for (int row = 0; row < grid_size; ++row)
		{
			for (int col = 0; col < grid_size; ++col)
			{
				float left = (col + 0) / float(grid_size) * 2.f - 1.f;
				float right = (col + 1) / float(grid_size) * 2.f - 1.f;
				float bottom = (row + 0) / float(grid_size) * 2.f - 1.f;
				float top = (row + 1) / float(grid_size) * 2.f - 1.f;

				vec3f quad[4] =
				{
					{ left, bottom, 1.f },
					{ right, bottom, 1.f },
					{ left, top, 1.f },
					{ right, top, 1.f }
				};

				vertex[0] = vertex[1] = vertex[2] = {};

				auto setPos = [sideMasks, sideSigns](int side, AssimpVertex& dst, const vec3f& pos)
				{
					dst.coor[sideMasks[side][0]] = pos.x() * sideSigns[side][0];
					dst.coor[sideMasks[side][1]] = pos.y() * sideSigns[side][1];
					dst.coor[sideMasks[side][2]] = pos.z() * sideSigns[side][2];
					dst.coor = dst.coor.normalized();
				};

				setPos(side, vertex[0], quad[0]);
				setPos(side, vertex[1], quad[1]);
				setPos(side, vertex[2], quad[2]);

				{
					vec3f AB = vertex[1].coor - vertex[0].coor;
					vec3f AC = vertex[2].coor - vertex[0].coor;
					vertex[0].normal = vertex[1].normal = vertex[2].normal = AC.cross(AB).normalized();
				}

				vertex += 3;

				setPos(side, vertex[0], quad[1]);
				setPos(side, vertex[1], quad[3]);
				setPos(side, vertex[2], quad[2]);

				{
					vec3f AB = vertex[1].coor - vertex[0].coor;
					vec3f AC = vertex[2].coor - vertex[0].coor;
					vertex[0].normal = vertex[1].normal = vertex[2].normal = AC.cross(AB).normalized();
				}

				vertex += 3;
			}
		}
	}

	for (int i = 0; i < cube_sphere.indices.size(); ++i)
	{
		cube_sphere.indices[i] = i;
	}

	cube_sphere.octrees.emplace_back();
	cube_sphere.octrees.back().initialize(cube_sphere.meshes.back());

	cube_sphere.vertexBuffer.write(cube_sphere.vertices.data(), cube_sphere.vertices.size() * sizeof(AssimpVertex));
	cube_sphere.indexBuffer.write(cube_sphere.indices.data(), cube_sphere.indices.size() * sizeof(uint32_t));
	
	models.insert({ model_name, cube_sphere_model });
}

void ModelManager::init_sphere(uint32_t sectors, uint32_t stacks)
{
	const char* model_name = "Sphere";
	if (models.count(model_name)) return;

	uint32_t num_vertices = (stacks + 1) * (sectors + 1);
	uint32_t num_indices = (stacks - 1) * sectors * 6;

	std::shared_ptr<Model> sphere_model{ new Model };
	Model& sphere = *sphere_model.get();

	sphere.name = "Sphere";
	sphere.tree = {
		{mat4f::Identity(), mat4f::Identity()}
	};

	sphere.meshes = {
		{ &sphere, {0}, {num_vertices, num_indices, 0, 0}, {{-1.f, -1.f, -1.f}, {1.f,  1.f,  1.f}} }
	};

	sphere.vertices.resize(num_vertices);
	sphere.indices.resize(num_indices);
	AssimpVertex* pvertex = sphere.vertices.data();
	uint32_t* pindex = sphere.indices.data();

	float rc;

	float stack_angle = PI / 2.f;
	float sector_angle;

	float stack_step = PI / stacks;
	float sector_step = 2 * PI / sectors;

	for (uint32_t stack = 0; stack <= stacks; ++stack)
	{
		rc = cosf(stack_angle);

		sector_angle = 0;
		for (uint32_t sector = 0; sector <= sectors; ++sector, ++pvertex)
		{
			pvertex->coor = { rc * sinf(sector_angle),  sinf(stack_angle), rc * cosf(sector_angle) };
			pvertex->normal = pvertex->coor;
			pvertex->tangent = { 1.f, 0.f, 0.f };
			pvertex->bitangent = { 0.f, - 1.f, 0.f };

			if (stack == 0)
				pvertex->texcoor = { (sector - 0.5f) / float(sectors), stack / float(stacks) };
			else if (stack == stacks)
				pvertex->texcoor = { (sector + 0.5f) / float(sectors), stack / float(stacks) };
			else
				pvertex->texcoor = { sector / float(sectors), stack / float(stacks) };

			sector_angle -= sector_step;
		}

		stack_angle -= stack_step;
	}

	auto add_indices = [&pindex](uint32_t i1, uint32_t i2, uint32_t i3)
	{
		pindex[0] = i1;
		pindex[1] = i2;
		pindex[2] = i3;
		pindex += 3;
	};

	uint32_t k1, k2;
	for (uint32_t stack = 0; stack < stacks; ++stack)
	{
		k1 = stack * (sectors + 1);
		k2 = k1 + sectors + 1;

		for (uint32_t sector = 0; sector < sectors; ++sector, ++k1, ++k2)
		{
			if (stack != 0)
				add_indices(k2, k1, k1 + 1);

			if (stack != (stacks - 1))
				add_indices(k2, k1 + 1, k2 + 1);
		}
	}

	sphere.octrees.emplace_back();
	sphere.octrees.back().initialize(sphere.meshes.back());

	sphere.vertexBuffer.write(sphere.vertices.data(), sphere.vertices.size() * sizeof(AssimpVertex));
	sphere.indexBuffer.write(sphere.indices.data(), sphere.indices.size() * sizeof(uint32_t));

	models.insert({ model_name, sphere_model});
}

void ModelManager::init_flat_sphere(uint32_t sectors, uint32_t stacks)
{
	const char* model_name = "FlatSphere";
	if (models.count(model_name)) return;

	uint32_t num_vertices = (4u * stacks - 2u) * sectors,
		num_indices = 6u * sectors * (stacks - 1u);

	std::shared_ptr<Model> sphere_model{ new Model };
	Model& sphere = *sphere_model.get();

	sphere.name = model_name;
	sphere.tree = {
		{mat4f::Identity(), mat4f::Identity()}
	};
	sphere.meshes = {
		{ &sphere, {0}, {num_vertices, num_indices, 0, 0}, {{-1.f, -1.f, -1.f}, {1.f,  1.f,  1.f}} }
	};


	sphere.vertices.resize(num_vertices);
	sphere.indices.resize(num_indices);
	AssimpVertex* pvertex = sphere.vertices.data();
	uint32_t* pindex = sphere.indices.data();

	auto add_indices = [&pindex](uint32_t i1, uint32_t i2, uint32_t i3)
	{
		pindex[0] = i1;
		pindex[1] = i2;
		pindex[2] = i3; 
		pindex += 3;
	};

	auto calc_vertex = [stacks, sectors](uint32_t index) -> AssimpVertex
	{
		float stack_step = PI / stacks;
		float sector_step = 2 * PI / sectors;
		uint32_t stack = index / (sectors + 1), sector = index - stack * (sectors + 1);

		float stack_angle = PI / 2.f - stack * stack_step;
		float sector_angle = sector * -sector_step;

		AssimpVertex vertex;
		float cos_stack = cosf(stack_angle);
		vertex.coor = { cos_stack * sinf(sector_angle), sinf(stack_angle), cos_stack * cosf(sector_angle) };
		vertex.tangent = { 1.f, 0.f, 0.f };
		vertex.bitangent = { 0.f, -1.f, 0.f };

		if (stack == 0)
			vertex.texcoor = { (sector + 0.5f) / sectors, float(stack) / stacks };
		else if (stack == stacks)
			vertex.texcoor = { (sector - 0.5f) / sectors, float(stack) / stacks };
		else
			vertex.texcoor = { sector / float(sectors), stack / float(stacks) };

		return vertex;
	};

	// vi -- vi1
	// |   /  |
	// |  /   |
	// vk -- vk1
	AssimpVertex vi, vi1, vk, vk1;

	uint32_t vi_ind = 0, vk_ind = (sectors + 1);
	uint32_t index = 0;

	for (uint32_t sector = 0; sector < sectors; ++sector, ++vi_ind, ++vk_ind)
	{
		vi1 = calc_vertex(vi_ind + 1);
		vk = calc_vertex(vk_ind);
		vk1 = calc_vertex(vk_ind + 1);

		vk.normal = (vi1.coor - vk.coor).cross(vk1.coor - vk.coor).normalized();
		vi1.normal = (vk1.coor - vi1.coor).cross(vk.coor - vi1.coor).normalized();
		vk1.normal = (vk.coor - vk1.coor).cross(vi1.coor - vk1.coor).normalized();

		pvertex[0] = vk;
		pvertex[1] = vi1;
		pvertex[2] = vk1;
		pvertex += 3;

		add_indices(index, index + 1, index + 2);
		index += 3;
	}

	for (uint32_t stack = 1; stack < stacks - 1; ++stack)
	{
		vi_ind++; vk_ind++;

		for (uint32_t sector = 0; sector < sectors; ++sector, ++vi_ind, ++vk_ind)
		{
			vi = calc_vertex(vi_ind);
			vi1 = calc_vertex(vi_ind + 1);
			vk = calc_vertex(vk_ind);
			vk1 = calc_vertex(vk_ind + 1);

			vk.normal = (vi.coor - vk.coor).cross(vk1.coor - vk.coor).normalized();
			vi.normal = (vi1.coor - vi.coor).cross(vk.coor - vi.coor).normalized();
			vi1.normal = (vk1.coor - vi1.coor).cross(vi.coor - vi1.coor).normalized();
			vk1.normal = (vk.coor - vk1.coor).cross(vi1.coor - vk1.coor).normalized();

			pvertex[0] = vk;
			pvertex[1] = vi;
			pvertex[2] = vi1;
			pvertex[3] = vk1;
			pvertex += 4;

			add_indices(index, index + 1, index + 2);
			add_indices(index, index + 2, index + 3);
			index += 4;
		}
	}

	vi_ind++; vk_ind++;
	for (uint32_t sector = 0; sector < sectors; ++sector, ++vi_ind, ++vk_ind)
	{
		vk = calc_vertex(vk_ind);
		vi = calc_vertex(vi_ind);
		vi1 = calc_vertex(vi_ind + 1);

		vk.normal = (vi.coor - vk.coor).cross(vi1.coor - vk.coor).normalized();
		vi.normal = (vi1.coor - vi.coor).cross(vk.coor - vi.coor).normalized();
		vi1.normal = (vk.coor - vk1.coor).cross(vi.coor - vk1.coor).normalized();

		pvertex[0] = vk;
		pvertex[1] = vi;
		pvertex[2] = vi1;
		pvertex += 3;

		add_indices(index, index + 1, index + 2);
		index += 3;
	}

	sphere.octrees.emplace_back();
	sphere.octrees.back().initialize(sphere.meshes.back());

	sphere.vertexBuffer.write(sphere.vertices.data(), sphere.vertices.size() * sizeof(AssimpVertex));
	sphere.indexBuffer.write(sphere.indices.data(), sphere.indices.size() * sizeof(uint32_t));

	models.insert({ model_name, sphere_model });
}
