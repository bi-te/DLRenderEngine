#pragma once

#include "Lighting.h"
#include "Direct11/DynamicBuffer.h"

struct PointLightBuffer
{
	vec3f radiance;
	float radius;

	vec3f position;
	float padding;
};

struct SpotlightBuffer
{
	vec3f radiance;
	float radius;

	vec3f position;
	float cutOff;

	vec3f direction;
	float outerCutOff;
};

const uint32_t MAX_LIGHTS_NUMBER = 10;
struct LightBuffer
{
	vec3f ambient;
	uint32_t pointLightNum;

	DirectLight dirLight;
	uint32_t spotlightNum;

	PointLightBuffer pointLights[MAX_LIGHTS_NUMBER];
	SpotlightBuffer spotlights[MAX_LIGHTS_NUMBER];
};

class LightSystem
{
	static LightSystem* s_system;
	LightSystem() = default;

	LightSystem(const LightSystem& other) = delete;
	LightSystem(LightSystem&& other) noexcept = delete;
	LightSystem& operator=(const LightSystem& other) = delete;
	LightSystem& operator=(LightSystem&& other) noexcept = delete;

	DynamicBuffer lightBuffer{ D3D11_BIND_CONSTANT_BUFFER };

	DirectLight dirLight;
	std::vector<PointLight> pointLights;
	std::vector<Spotlight> spotlights;
	vec3f ambient;
public:

	void set_ambient(const vec3f& ambient_color);
	void set_direct_light(const DirectLight& dirLight);
	void add_point_light(const PointLight& pointLight);
	void add_point_light(const PointLight& pointLight, const std::string& model);
	void add_spotlight(const Spotlight& spotlight);
	void add_spotlight(const Spotlight& spotlight, const std::string& model);

	void bind_lights();

	static void init()
	{
		if (s_system) reset();

		s_system = new LightSystem;
	}

	static LightSystem& instance()
	{
		assert(s_system && "LightSystem not initialized");
		return *s_system;
	}

	static void reset()
	{
		delete s_system;
	}
};

