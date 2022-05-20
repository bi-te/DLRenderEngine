#pragma once

#include "../math/math.h"

enum MaterialType{LIGHT_SOURCE, SURFACE};

struct Material{
	MaterialType type;
	vec3 albedo;
	vec3 emission;
	float specular;
	float glossiness;
};
