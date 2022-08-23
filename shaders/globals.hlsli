#ifndef _GLOBALS_
#define _GLOBALS_

static const float PI = 3.14159265f;

struct Frustum
{
	float4 bottom_left;
	float4 up_vector;
	float3 right_vector;
};

cbuffer perFrame: register(b0)
{
	float4x4 g_viewProj;
	Frustum g_frustum;
	float3 g_cameraPosition;
}

SamplerState g_sampler: register(s0);

float3 irradianceAtDistanceToRadiance(float3 irradiance, float distance, float radius)
{
	float angleSin = min(1.f, radius / distance);
	float angleCos = sqrt(1.f - angleSin * angleSin);
	float occupation = 1.f - angleCos;
	return irradiance / occupation;
}

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

static const uint MAX_LIGHTS_NUMBER = 10;
cbuffer LightBuffer: register(b1)
{
	float3 ambient;
	uint pointLightNum;

	DirectLight dirLight;
	uint spotlightNum;

	PointLight pointLights[MAX_LIGHTS_NUMBER];
	Spotlight spotlights[MAX_LIGHTS_NUMBER];
}

#endif