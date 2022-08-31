#pragma once

#include "d3d.h"
#include "DynamicBuffer.h"
#include "math/math.h"
#include "math/Camera.h"
#include "render/Lighting.h"

struct Frustum
{
	vec4f bottom_left_point, up_vector,	right_vector;
};

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

	SpotlightBuffer spotlights[MAX_LIGHTS_NUMBER];
	PointLightBuffer pointLights[MAX_LIGHTS_NUMBER];
};

struct PerFrame
{
	mat4f view_projection;

	Frustum frustum;

	vec3f camera_pos;
	float padding0;

	LightBuffer light_buffer;
};

constexpr ID3D11RenderTargetView* NULL_RTV = nullptr;
constexpr ID3D11ShaderResourceView* NULL_SRV = nullptr;

class Direct3D
{
	static Direct3D* direct3d;
	Direct3D() = default;

	Direct3D(const Direct3D& other) = delete;
	Direct3D(Direct3D&& other) noexcept = delete;
	Direct3D& operator=(const Direct3D& other) = delete;
	Direct3D& operator=(Direct3D&& other) noexcept = delete;

public:
	comptr<IDXGIFactory5> factory5;
	comptr<ID3D11Device5> device5;
	comptr<ID3D11DeviceContext4> context4;
	comptr<ID3D11Debug> devdebug;

	comptr< ID3D11SamplerState> sampler_state;
	comptr<ID3D11RasterizerState> rasterizer_state;

	DynamicBuffer per_frame_buffer{ D3D11_BIND_CONSTANT_BUFFER };

	static void init();

    static Direct3D& instance()
    {
		assert(direct3d && "Direct3D not initialized");
        return *direct3d;
    }

	void init_core();
	void init_rasterizer_state();
	void init_sampler_state(D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR, uint8_t anisotropy = 0);

	void bind_globals(const Camera& camera);

	static void reset();
};
