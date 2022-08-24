#pragma once
#include "../math/math.h"
#include "data_structures/solid_vector.h"

struct DirectLight
{
	vec3f radiance;
	float solid_angle;

	vec3f direction;
};

struct PointLight
{
	vec3f irradiance;
	ID position;

	float radius;	
	float light_range;
	vec3f radiance;
};

struct Spotlight 
{
	vec3f irradiance;
	ID position;

	vec3f direction;
	float radius;

	float cutOff, outerCutOff;	
	float light_range;
	vec3f radiance;
};
