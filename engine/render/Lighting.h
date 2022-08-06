#pragma once
#include "../math/math.h"

struct DirectLight
{
	vec3f light;
	vec3f direction;
	float solid_angle;
};

struct PointLight
{
	vec3f light;
	vec3f position;

	//Phong
	float light_range;
};

struct Spotlight 
{
	vec3f light;
	vec3f position;
	vec3f direction;

	float cutOff, outerCutOff;

	//Phong
	float light_range;
};