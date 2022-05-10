#pragma once
#include "math/Vec3.h"

struct DirectLight
{
	Vec3 light;
	Vec3 direction;
};

struct PointLight
{
	Vec3 light;
	Vec3 position;
	float light_distance;
};

struct Spotlight 
{
	Vec3 light;
	Vec3 position;
	Vec3 direction;

	float light_distance;
	float cutOff, outerCutOff;
};