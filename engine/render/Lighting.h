#pragma once
#include "../math/math.h"

struct DirectLight
{
	vec3 light;
	vec3 direction;
	float solid_angle;
};

struct PointLight
{
	vec3 light;
	vec3 position;

	//Phong
	float light_range;
};

struct Spotlight 
{
	vec3 light;
	vec3 position;
	vec3 direction;

	float cutOff, outerCutOff;

	//Phong
	float light_range;
};