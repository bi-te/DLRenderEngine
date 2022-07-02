#pragma once

#include "../math/math.h"

enum MaterialType{LIGHT_SOURCE, SURFACE};

struct Material{
	MaterialType type;
	vec3 albedo;
	vec3 emission;

	// Phong
	float specular;
	float glossiness;

	// PBR
	vec3 f0;
	float roughness;
	float metalness;
};
