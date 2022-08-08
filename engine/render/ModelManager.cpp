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

	std::vector<AssimpVertex> vertices;
	std::vector <uint32_t> indices;

	uint32_t vsize = 0, isize = 0;
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh& mesh = *scene->mMeshes[i];
		Model::MeshRange& m_data = model.meshes[i];
		
		m_data.verticesOffset = vsize;
		m_data.indicesOffset = isize;

		for (int i = 0; i < mesh.mNumVertices; ++i, ++vsize)
		{
			AssimpVertex vertex;
			vertex.coor = reinterpret_cast<vec3f&>(mesh.mVertices[i]);
			vertex.texcoor = reinterpret_cast<vec2f&>(mesh.mTextureCoords[0][i]);
			vertex.normal = reinterpret_cast<vec3f&>(mesh.mNormals[i]);
			vertex.tangent = reinterpret_cast<vec3f&>(mesh.mTangents[i]);
			vertex.bitangent = reinterpret_cast<vec3f&>(mesh.mBitangents[i]);
			vertices.push_back(vertex);
		}

		for (int i = 0; i < mesh.mNumFaces; ++i, isize += 3)
		{
			aiFace& face = mesh.mFaces[i];
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		m_data.numVertices = vsize - m_data.verticesOffset;
		m_data.numIndices = isize - m_data.indicesOffset;

	}

	for (int i = 0; i < scene->mNumMaterials; ++i)
	{
		load_material(*scene->mMaterials[i], dir);
	}

	model.vertexBuffer.write(vertices.data(), vsize * sizeof(AssimpVertex), Direct3D::instance().device5);
	model.indexBuffer.write(indices.data(), isize * sizeof(uint32_t), Direct3D::instance().device5);

	models.insert({ path, s_model });

	parse_tree(*scene, model);
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

std::wstring ModelManager::load_texture(aiTextureType type, aiMaterial& material,
	const std::string& directory)
{
	aiString texture;
	if (!material.GetTextureCount(type)) return {};

	material.GetTexture(type, 0, &texture);

	char path[256]{};
	strcat_s(path, directory.c_str());
	strcat_s(path, texture.C_Str());

	wchar_t* wtexture = char_to_wchar(path);
	TextureManager::instance().add_texture(wtexture);
	return { wtexture };
}

Material ModelManager::load_material(aiMaterial& material, std::string& path)
{
	Material mat;
	mat.name = path + material.GetName().C_Str(),
	mat.diffuse = load_texture(aiTextureType_DIFFUSE, material, path);
	mat.normals = load_texture(aiTextureType_NORMALS, material, path);
	mat.metallic = load_texture(aiTextureType_METALNESS, material, path);
	mat.roughness = load_texture(aiTextureType_DIFFUSE_ROUGHNESS, material, path);

	MaterialManager::instance().add(mat.name, std::move(mat));

	return mat;
}

void ModelManager::parse_tree(const aiScene& scene, Model& model)
{
	std::vector<Model::Node>& tree = model.tree;
	std::queue<aiNode*> nodes;
	Model::Node root_node;

	std::vector<mat4f> mesh_model;

	nodes.push(scene.mRootNode);
	
	root_node.mesh_matrix = mat4f::Identity();
	tree.push_back(root_node);

	for (uint32_t ind = 0, visited_nodes = 1; !nodes.empty(); ++ind)
	{
		aiNode& node = *nodes.front(); nodes.pop();
		tree.resize(tree.size() + node.mNumChildren);

		Model::Node& tree_node = tree.at(ind);

		tree_node.mesh_matrix *= reinterpret_cast<mat4f&>(node.mTransformation.Transpose());
		if(ind != 0)
			mesh_model.push_back(tree_node.mesh_matrix);

		for (int i = 0; i < node.mNumMeshes; ++i)
			tree_node.meshes.push_back(node.mMeshes[i]);

		
		for (uint32_t child_ind = 0; child_ind < node.mNumChildren; ++child_ind)
		{
			uint32_t child_node = visited_nodes + child_ind;
			tree.at(child_node).mesh_matrix = tree_node.mesh_matrix;

			nodes.push(node.mChildren[child_ind]);
		}

		visited_nodes += node.mNumChildren;
	}
}

void ModelManager::make_cube()
{
	if (models.count("Cube")) return;

	Model cube;
	cube.name = "Cube";
	cube.tree = {
		{mat4f::Identity(), {}}, {mat4f::Identity(), {0}}
	};

	cube.meshes = {
		{36, 24, 0, 0}
	};

	AssimpVertex vertices[24]{
	{{ 0.5f, -0.5f,  0.5f},   {0.f,  0.f}, { 0.f,  0.f,  1.f}},
	{{ 0.5f,  0.5f,  0.5f},   {0.f,  1.f}, { 0.f,  0.f,  1.f}},
	{{-0.5f,  0.5f,  0.5f},   {1.f,  1.f}, { 0.f,  0.f,  1.f}},
	{{-0.5f, -0.5f,  0.5f},   {1.f,  0.f}, { 0.f,  0.f,  1.f}},

	{{-0.5f, -0.5f, -0.5f},   {0.f,  0.f}, { 0.f,  0.f, -1.f}},
	{{-0.5f,  0.5f, -0.5f},   {0.f,  1.f}, { 0.f,  0.f, -1.f}},
	{{ 0.5f,  0.5f, -0.5f},   {1.f,  1.f}, { 0.f,  0.f, -1.f}},
	{{ 0.5f, -0.5f, -0.5f},   {1.f,  0.f}, { 0.f,  0.f, -1.f}},

	{{ 0.5f, -0.5f, -0.5f},   {0.f,  0.f}, { 1.f,  0.f,  0.f}},
	{{ 0.5f,  0.5f, -0.5f},   {0.f,  1.f}, { 1.f,  0.f,  0.f}},
	{{ 0.5f,  0.5f,  0.5f},   {1.f,  1.f}, { 1.f,  0.f,  0.f}},
	{{ 0.5f, -0.5f,  0.5f},   {1.f,  0.f}, { 1.f,  0.f,  0.f}},

	{{-0.5f, -0.5f,  0.5f},   {0.f,  0.f}, {-1.f,  0.f,  0.f}},
	{{-0.5f,  0.5f,  0.5f},   {0.f,  1.f}, {-1.f,  0.f,  0.f}},
	{{-0.5f,  0.5f, -0.5f},   {1.f,  1.f}, {-1.f,  0.f,  0.f}},
	{{-0.5f, -0.5f, -0.5f},   {1.f,  0.f}, {-1.f,  0.f,  0.f}},

	{{-0.5f,  0.5f, -0.5f},   {0.f,  0.f}, { 0.f,  1.f,  0.f}},
	{{-0.5f,  0.5f,  0.5f},   {0.f,  1.f}, { 0.f,  1.f,  0.f}},
	{{ 0.5f,  0.5f,  0.5f},   {1.f,  1.f}, { 0.f,  1.f,  0.f}},
	{{ 0.5f,  0.5f, -0.5f},   {1.f,  0.f}, { 0.f,  1.f,  0.f}},

	{{ 0.5f, -0.5f, -0.5f},   {0.f,  0.f}, { 0.f, -1.f,  0.f}},
	{{ 0.5f, -0.5f,  0.5f},   {0.f,  1.f}, { 0.f, -1.f,  0.f}},
	{{-0.5f, -0.5f,  0.5f},   {1.f,  1.f}, { 0.f, -1.f,  0.f}},
	{{-0.5f, -0.5f, -0.5f},   {1.f,  0.f}, { 0.f, -1.f,  0.f}},
	};

	uint32_t indices[36] = {
		0,  1,   2,  0,  2,  3,
		4,  5,   6,  4,  6,  7,
		8,  9,  10,  8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	cube.vertexBuffer.write(vertices, ARRAYSIZE(vertices) * sizeof(AssimpVertex), Direct3D::instance().device5);
	cube.indexBuffer.write(indices, ARRAYSIZE(indices) * sizeof(uint32_t), Direct3D::instance().device5);

	models.insert({ "Cube", std::make_shared<Model>(cube) });
}
