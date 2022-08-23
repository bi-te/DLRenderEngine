#pragma once

#include "../math/math.h"

const float BASE_ROUGHNESS = 0.7f;
const float BASE_METALLIC  = 0.f;

enum MaterialType{LIGHT_SOURCE, SURFACE};

struct OldMaterial{
	MaterialType type;
	vec3f albedo;
	vec3f emission;

	// Phong
	float specular;
	float glossiness;

	// PBR
	vec3f f0;
	float roughness;
	float metalness;
};

struct BufferMaterial
{
	uint32_t hasDiffuseTexture;
	uint32_t hasNormalsTexture;
	uint32_t hasRoughnessTexture;
	uint32_t hasMetallicTexture;

	vec3f diffuse;	
	float roughness;
	
	float metallic;
	uint32_t reverseNormalTextureY;
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
		render_data.hasNormalsTexture = render_data.hasDiffuseTexture = 
			render_data.hasMetallicTexture = render_data.hasRoughnessTexture = false;
		render_data.metallic = render_data.roughness = 0.f;
		render_data.diffuse = {};
	}
};