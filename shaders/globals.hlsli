#ifndef _GLOBALS_
#define _GLOBALS_

static const float PI = 3.14159265f;
static const float offset = 0.02f;

struct Frustum
{
	float4 bottom_left;
	float4 up_vector;
	float4 right_vector;
};

struct DirectLight
{
	float3 radiance;
	float solid_angle;

	float3 direction;
};

struct PointLight
{
	float3 radiance;
	float radius;

	float3 position;
	float padding;
};

struct Spotlight
{
	float3 radiance;
	float radius;

	float3 position;
	float cutOff;

	float3 direction;
	float outerCutOff;
};

struct LightTransBuffer
{
	//vec4f light_pos;
	float4x4 light_view[6];
	float4x4 light_proj;
};

static const uint MAX_LIGHTS_NUMBER = 10;
struct LightBuffer
{
	float3 ambient;
	uint pointLightNum;

	DirectLight dirLight;
	uint spotlightNum;

	Spotlight spotlights[MAX_LIGHTS_NUMBER];
	PointLight pointLights[MAX_LIGHTS_NUMBER];
	LightTransBuffer pointTrans[MAX_LIGHTS_NUMBER];

	float shadow_near, shadow_far;
	float buffer_side;
	float padding;
};

SamplerState g_sampler: register(s0);
SamplerState g_linear_clamp_sampler: register(s1);
SamplerComparisonState g_comparison_sampler: register(s2);

Texture2D g_reflectance: register(t0);
TextureCube g_irradiance: register(t1);
TextureCube g_reflection: register(t2);
TextureCubeArray g_shadows:register(t3);

cbuffer perFrame: register(b0)
{
	float4x4 g_viewProj;
	Frustum g_frustum;

	float3 g_cameraPosition;
	float padding0;

	LightBuffer g_lighting;
}
#endif