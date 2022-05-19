#pragma once
#include "../math/math.h"

struct DirectLight
{
	vec3 light;
	vec3 direction;
};

struct PointLight
{
	vec3 light;
	vec3 position;
	float light_distance;
};

struct Spotlight 
{
	vec3 light;
	vec3 position;
	vec3 direction;

	float light_distance;
	float cutOff, outerCutOff;
};