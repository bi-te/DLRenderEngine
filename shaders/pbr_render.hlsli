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

float3 cook_torrance_aprox(float3 light, float3 closest_light, float3 normal, float3 view,
	float3 radiance, float solid_angle, Material mat)
{
	float3 h = normalize(closest_light + view);
	float3 f0 = lerp(INSULATOR_F0, mat.diffuse, mat.metallic);

	float cosNV = max(saturate(dot(normal, view)), 0.001f);
	float cosNL = dot(normal, light);
	if (cosNL <= 0.f) { return 0.f; }
	float cosNC = max(dot(normal, closest_light), 0.001f);
	float cosNH = max(dot(normal, h), 0.001f);
	float cosHL = max(dot(h, closest_light), 0.001f);

	float rough2 = mat.roughness * mat.roughness;

	float g = ggx_smith(rough2, cosNV, cosNC);
	float d = min(ggx_distribution(rough2, cosNH) * solid_angle * 0.25f / (cosNV * cosNL), 1.f);
	float3 f = fresnel(f0, cosHL);
	float3 diffK = float3(1.f, 1.f, 1.f) - fresnel(f0, cosNL);

	float3 spec = f * g * d;
	float3 diff = (1 - mat.metallic) * diffK * (mat.diffuse) * solid_angle / PI;

	float3 color = (diff + spec) * (radiance)*cosNL;
	return color;
}
#endif