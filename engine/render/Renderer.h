#pragma once

#include <iostream>

#include "Direct11/Direct3D.h"
#include "Direct11/DynamicBuffer.h"
#include "math/math.h"

struct DepthStencil
{
    comptr<ID3D11Texture2D> buffer;
    comptr<ID3D11DepthStencilView> view;
    comptr<ID3D11DepthStencilState> state;

    void reset()
    {
        buffer.Reset();
        view.Reset();
        state.Reset();
    }
};

class Renderer
{
    uint32_t scbuffer_width, scbuffer_height;

    DepthStencil depth_stencil;
    comptr< ID3D11SamplerState> sampler_state;
    comptr<ID3D11RasterizerState> rasterizer_state;
    comptr<ID3D11RenderTargetView> target_view;
    comptr<IDXGISwapChain1> swap_chain;
    DynamicBuffer<D3D11_BIND_CONSTANT_BUFFER> view_projection_buffer;
public:

	Renderer()
	{
        view_projection_buffer.allocate(sizeof(mat4));
	}

    uint32_t buffer_width() const { return scbuffer_width; }
    uint32_t buffer_height() const { return scbuffer_height; }

    void init(HWND window);
    void init_swap_chain(HWND window);
    void init_render_target_view();
    void init_depth_and_stencil_buffer();
    void init_depth_stencil_state();
    void init_rasterizer_state();
    void init_sampler_state(D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR);
    void resize_buffers(uint32_t width, uint32_t height);

    void bind_globals(const mat4& viewProj);
    void clear_buffers(const float background_color[4]);
    void prepare_output();
    void flush() { swap_chain->Present(0, 0); }

    void reset()
    {
        depth_stencil.reset();
        sampler_state.Reset();
        rasterizer_state.Reset();
        view_projection_buffer.free();
        target_view.Reset();
        swap_chain.Reset();
    }
};
