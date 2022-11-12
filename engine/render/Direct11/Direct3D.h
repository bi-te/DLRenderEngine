#pragma once

#include "d3d.h"
#include "DynamicBuffer.h"
#include "Texture.h"
#include "math/math.h"
#include "math/Camera.h"
#include "render/Lighting.h"
#include "render/ShaderManager.h"

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
	float cutOffCos;

	vec3f direction;
	float outerCutOffCos;
};

struct PointLightTransBuffer
{
	mat4f light_view_proj[6];
};

struct SpotlightTransBuffer
{
	mat4f light_view_proj;

	float fov_tan;
	float padding[3];
};


const uint32_t MAX_LIGHTS_NUMBER = 10;
struct LightBuffer
{
	vec3f ambient;
	uint32_t pointLightNum;

	DirectLight dirLight;
	uint32_t spotlightNum;

	SpotlightBuffer spotlights[MAX_LIGHTS_NUMBER];
	SpotlightTransBuffer spotTrans[MAX_LIGHTS_NUMBER];
	PointLightBuffer pointLights[MAX_LIGHTS_NUMBER];
	PointLightTransBuffer pointTrans[MAX_LIGHTS_NUMBER];

	float shadow_near, shadow_far;
	float buffer_side_size;
	float padding;
};

struct PerFrame
{
	mat4f view_projection;

	Frustum frustum;

	vec3f camera_pos;
	uint32_t max_reflection_mip;

	LightBuffer light_buffer;

	float near, far;
	float time, frame_time;
};

struct ResolveBuffer
{
	uint32_t msaa;
	float padding[3];
};

constexpr ID3D11RenderTargetView* NULL_RTV = nullptr;
constexpr ID3D11ShaderResourceView* NULL_SRV = nullptr;
constexpr ID3D11UnorderedAccessView* NULL_UAV = nullptr;
constexpr ID3D11Buffer* NULL_BUFFER = nullptr;

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

	comptr<ID3D11SamplerState> sampler_state;
	comptr<ID3D11SamplerState> linear_clamp_sampler_state;
	comptr<ID3D11SamplerState> comparison_sampler_state;

	comptr<ID3D11BlendState> blend_state;
	comptr<ID3D11BlendState> atc_blend_state;
	comptr<ID3D11BlendState> additive_blend_state;

	comptr<ID3D11RasterizerState> rasterizer_state;
	comptr<ID3D11RasterizerState> two_face_rasterizer_state;
	comptr<ID3D11RasterizerState> back_face_rasterizer_state;
	std::shared_ptr<Texture> reflectance_map;

	std::shared_ptr<Shader> depth_resolve_shader;

	DynamicBuffer per_frame_buffer{ D3D11_BIND_CONSTANT_BUFFER };
	DynamicBuffer resolve_buffer{D3D11_BIND_CONSTANT_BUFFER}; 

	static void init();

    static Direct3D& instance()
    {
		assert(direct3d && "Direct3D not initialized");
        return *direct3d;
    }

	void resolve_depth(comptr<ID3D11ShaderResourceView> msaa_depth, comptr<ID3D11DepthStencilView> target, uint32_t msaa);

	void init_core();
	void init_rasterizer_states();
	void init_blend_state();
	void init_alpha_to_coverage();
	void init_sampler_state(D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR, uint8_t anisotropy = 0);
	void init_linear_clamp_sampler();
	void init_comparison_sampler();

	void bind_globals(const Camera& camera, uint32_t max_reflection_mip);

	static void reset();
};
