#pragma once

#include "d3d.h"
#include "DynamicBuffer.h"
#include "math/math.h"

struct Frustum
{
	vec4f bottom_left_point,
		up_vector,
		right_vector;
};

struct PerFrame
{
	mat4f view_projection;
	Frustum frustum;
};

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

	DynamicBuffer<D3D11_BIND_CONSTANT_BUFFER> per_frame_buffer;

	static void init()
	{
		if (direct3d) reset();

		direct3d = new Direct3D;
		direct3d->init_core();
		direct3d->init_rasterizer_state();
		direct3d->init_sampler_state();
		direct3d->per_frame_buffer.allocate(sizeof(PerFrame), direct3d->device5);
	}

    static Direct3D& instance()
    {
		assert(direct3d && "Direct3D not initialized");
        return *direct3d;
    }

	void init_core();
	void init_rasterizer_state();
	void init_sampler_state(D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR, uint8_t anisotropy = 0);

	void bind_globals(const PerFrame& per_frame_data);

	static void reset();
};
