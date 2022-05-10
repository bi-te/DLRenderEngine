#pragma once
#include "math/Vec3.h"

struct Material{
	Vec3 albedo;
	Vec3 emission;
	float specular;
	float glossiness;
};
