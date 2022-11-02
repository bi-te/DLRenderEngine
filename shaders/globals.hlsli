#ifndef _GLOBALS_
#define _GLOBALS_

static const float PI = 3.14159265f;
static const float OFFSET = 0.02f;

static const float3 X_VEC = float3(1.f, 0.f, 0.f);
static const float3 Y_VEC = float3(0.f, 1.f, 0.f);
static const float3 Z_VEC = float3(0.f, 0.f, 1.f);
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
	float cutOffCos;

	float3 direction;
	float outerCutOffCos;
};

struct PointLightTransBuffer
{
	float4x4 light_view_proj[6];
};

struct SpotlightTransBuffer
{
	float4x4 light_view_proj;
	float fov_tan;
};

static const uint MAX_LIGHTS_NUMBER = 10;
struct LightBuffer
{
	float3 ambient;
	uint pointLightNum;

	DirectLight dirLight;
	uint spotlightNum;

	Spotlight spotlights[MAX_LIGHTS_NUMBER];
	SpotlightTransBuffer spotTrans[MAX_LIGHTS_NUMBER];
	PointLight pointLights[MAX_LIGHTS_NUMBER];
	PointLightTransBuffer pointTrans[MAX_LIGHTS_NUMBER];

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
Texture2DArray g_spot_shadows: register(t4);

cbuffer perFrame: register(b0)
{
	float4x4 g_viewProj;
	Frustum g_frustum;

	float3 g_cameraPosition;
	uint g_max_reflection_mip;

	LightBuffer g_lighting;

	float g_near, g_far;
	float g_time;
}

float world_depth_from_buffer(float buffer_data)
{
    return -g_near * g_far / (buffer_data * (g_near - g_far) - g_near);
}
#endif