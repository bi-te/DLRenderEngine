#pragma once

#include "../math/math.h"

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

struct Material
{
	std::string name;
	std::wstring diffuse;
	std::wstring normals;
	std::wstring metallic;
	std::wstring roughness;
};
