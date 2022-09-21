#pragma once

#include "Lighting.h"
#include "Direct11/Direct3D.h"
#include "Direct11/DynamicBuffer.h"

struct ShadowBuffer
{
	uint32_t index;
	float padding[3];
};

class LightSystem
{
	struct DepthBuffer
	{
		D3D11_VIEWPORT viewport;
		comptr<ID3D11DepthStencilView> point_view;
		comptr<ID3D11DepthStencilView> spot_view;
		comptr<ID3D11ShaderResourceView> point_srv;
		comptr<ID3D11ShaderResourceView> spot_srv;
		comptr<ID3D11DepthStencilState> state;
	};

	static LightSystem* s_system;
	LightSystem()
	{
		depthBuffer.viewport = { 0.f, 0.f, 0.f, 0.f, 0.f, 1.f };
		lightTransformBuffer.allocate(sizeof(ShadowBuffer));
	}

	LightSystem(const LightSystem& other) = delete;
	LightSystem(LightSystem&& other) noexcept = delete;
	LightSystem& operator=(const LightSystem& other) = delete;
	LightSystem& operator=(LightSystem&& other) noexcept = delete;

	DynamicBuffer lightBuffer{ D3D11_BIND_CONSTANT_BUFFER };
	DynamicBuffer lightTransformBuffer{ D3D11_BIND_CONSTANT_BUFFER };

	DirectLight dirLight;
	std::vector<PointLight> pointLights;
	std::vector<Spotlight> spotlights;
	vec3f ambient;
public:
	float shadow_near = 0.1f, shadow_far = 100.f;
	DepthBuffer depthBuffer;

	void set_ambient(const vec3f& ambient_color);
	void set_direct_light(const DirectLight& dirLight);
	void add_point_light(const PointLight& pointLight);
	void add_point_light(const PointLight& pointLight, const std::string& model);
	void add_spotlight(const Spotlight& spotlight);
	void add_spotlight(const Spotlight& spotlight, const std::string& model);

	void init_depth_buffers(uint32_t side_size);

	const std::vector<Spotlight>& slights() const { return spotlights; }
	const std::vector<PointLight>& plights() const { return pointLights; }

	void bind_point_dsv();
	void bind_light_shadow_buffer(uint32_t index);
	void bind_spot_dsv();
	void bind_lights(LightBuffer* lBuffer);
	void bind_depth_state();

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

