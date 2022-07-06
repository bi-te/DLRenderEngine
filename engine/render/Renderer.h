#pragma once

#include <iostream>

#include "Direct3D.h"
#include "../Scene.h"

enum ShadersType{VertexShader, PixelShader};

class Renderer
{
    comptr<IDXGISwapChain1> swap_chain;
    comptr<ID3D11RenderTargetView> target_view;

    comptr<ID3DBlob> vertex_shader_blob, pixel_shader_blob, error_blob;
    comptr<ID3D11VertexShader> vertex_shader;
    comptr<ID3D11PixelShader> pixel_shader;

    comptr<ID3D11InputLayout> layout;
public:
	Renderer()
	{
	}

    void init_swap_chain(HWND window);
    void init_render_target_view();
    void resize_buffers();
    
    void update_vertex_shader(LPCWSTR file_name, LPCSTR entry_point);
    void update_pixel_shader(LPCWSTR file_name, LPCSTR entry_point);

    void create_input_layout();

    void draw(Scene& scene);

    void clear()
    {
        layout.Reset();
        pixel_shader.Reset();
        vertex_shader.Reset();
        vertex_shader_blob.Reset();
        pixel_shader_blob.Reset();
        error_blob.Reset();
        target_view.Reset();
        swap_chain.Reset();
    }
};