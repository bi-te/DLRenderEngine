#pragma once

#include "../math/math.h"

const float BASE_ROUGHNESS = 0.7f;
const float BASE_METALLIC  = 0.f;

enum MaterialType{LIGHT_SOURCE, SURFACE};

enum MaterialTexture : uint32_t
{
	MATERIAL_TEXTURE_DIFFUSE = 1,
	MATERIAL_TEXTURE_NORMAL = 2,
	MATERIAL_TEXTURE_ROUGHNESS = 4,
	MATERIAL_TEXTURE_METALLIC = 16,
	MATERIAL_REVERSED_NORMAL_Y = 32
};

struct BufferMaterial
{
	vec3f diffuse;
	uint32_t textures;
		
	float roughness;	
	float metallic;
	float padding[2];
};

struct OpaqueMaterial
{
	std::string name;
	std::wstring diffuse;
	std::wstring normals;
	std::wstring metallic;
	std::wstring roughness;

	BufferMaterial render_data;

	void clear()
	{
		name = "";
		diffuse = normals = metallic = roughness = L"";
		render_data.textures = 0;
		render_data.metallic = render_data.roughness = 0.f;
		render_data.diffuse = {};
	}
};