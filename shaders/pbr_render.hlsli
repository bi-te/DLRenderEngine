#ifndef _PBR_RENDER_
#define _PBR_RENDER_
#include "globals.hlsli"

static const float INSULATOR_F0 = 0.04f;

#define MATERIAL_TEXTURE_DIFFUSE 1u
#define MATERIAL_TEXTURE_NORMAL 2u
#define MATERIAL_TEXTURE_ROUGHNESS 4u
#define MATERIAL_TEXTURE_METALLIC 16u
#define REVERSED_NORMAL_Y 32u

struct Material
{
	float3 diffuse;
	uint textures;

	float roughness;
	float metallic;
	float2 padding0;
};

float ggx_distribution(float rough2, float cosNH)
{
	float denum = cosNH * cosNH * (rough2 - 1) + 1;
	denum = PI * denum * denum;
	return rough2 / denum;
}

float ggx_smith(float rough2, float cosNV, float cosNL)
{
	cosNV *= cosNV;
	cosNL *= cosNL;
	return 2 / (sqrt(1 + rough2 * (1 - cosNV) / cosNV) + sqrt(1 + rough2 * (1 - cosNL) / cosNL));
}

float3 fresnel(float3 f0, float cosNL)
{
	return f0 + (1.f - f0) * pow(1.f - cosNL, 5.f);
}

#endif