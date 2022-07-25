#include "Renderer.h"

#include "ShaderManager.h"
#include "TextureManager.h"
#include "Direct11/ImmutableBuffer.h"

void Renderer::init(HWND window)
{
    init_swap_chain(window);
    init_render_target_view();
    init_depth_and_stencil_buffer();
    init_depth_stencil_state();
    init_rasterizer_state();
    init_sampler_state();
}

void Renderer::init_swap_chain(HWND window)
{
    Direct3D& globals = Direct3D::globals();
    DXGI_SWAP_CHAIN_DESC1 desc;
    ZeroMemory(&desc, sizeof(desc));

    RECT winrect;
    GetClientRect(window, &winrect);

    scbuffer_width = winrect.right - winrect.left;
    scbuffer_height = winrect.bottom - winrect.top;

    desc.Width = scbuffer_width;
    desc.Height = scbuffer_height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.Scaling = DXGI_SCALING_NONE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    desc.Flags = 0;
    desc.Stereo = false;

    HRESULT result = globals.factory5->CreateSwapChainForHwnd(globals.device5.Get(), window, &desc,
        NULL, NULL, &swap_chain);
    assert(result >= 0 && "CreateSwapChainForHwnd");
}

void Renderer::init_render_target_view()
{
    if (!swap_chain.Get())
        return;

    comptr<ID3D11Texture2D> back_buffer;
    HRESULT result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &back_buffer);
    assert(SUCCEEDED(result) && "GetBuffer");

    result = Direct3D::globals().device5->CreateRenderTargetView(back_buffer.Get(), NULL, &target_view);
    assert(SUCCEEDED(result) && "CreateRenderTargetView");
}

void Renderer::init_depth_and_stencil_buffer()
{
    D3D11_TEXTURE2D_DESC depthBufferDesc{};
    depthBufferDesc.Width = scbuffer_width;
    depthBufferDesc.Height = scbuffer_height;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    HRESULT result = Direct3D::globals().device5->CreateTexture2D(&depthBufferDesc, nullptr,
        &depth_stencil.buffer);
    assert(SUCCEEDED(result) && "CreateTexture2D Depth Stencil");

    D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc{};
    depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthViewDesc.Texture2D.MipSlice = 0;

    
    result = Direct3D::globals().device5->CreateDepthStencilView(depth_stencil.buffer.Get(),
        &depthViewDesc, &depth_stencil.view);
    assert(SUCCEEDED(result) && "CreateDepthStencilView");
}

void Renderer::init_depth_stencil_state()
{
    D3D11_DEPTH_STENCIL_DESC depthDesc{};
    depthDesc.DepthEnable = true;
    depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    HRESULT result = Direct3D::globals().device5->CreateDepthStencilState(&depthDesc, &depth_stencil.state);
    assert(SUCCEEDED(result) && "CreateDepthStencilState");
}

void Renderer::init_rasterizer_state()
{
    D3D11_RASTERIZER_DESC rasterizer{};
    rasterizer.FillMode = D3D11_FILL_SOLID;
    rasterizer.CullMode = D3D11_CULL_BACK;
    rasterizer.DepthClipEnable = true;
    rasterizer.FrontCounterClockwise = false;
    HRESULT result = Direct3D::globals().device5->CreateRasterizerState(&rasterizer, &rasterizer_state);
    assert(SUCCEEDED(result) && "CreateRasterizerState");
}

void Renderer::init_sampler_state(D3D11_FILTER filter)
{
    D3D11_SAMPLER_DESC sdesc{};
    sdesc.Filter = filter;
    sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.MinLOD = 0.f;
    sdesc.MaxLOD = 10.f;
    Direct3D::globals().device5->CreateSamplerState(&sdesc, &sampler_state);
}

void Renderer::resize_buffers(uint32_t width, uint32_t height)
{
    if (!swap_chain.Get())
        return;

    target_view.Reset();
    HRESULT result = swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
    assert(SUCCEEDED(result) && "ResizeBuffers");

    scbuffer_width = width;
    scbuffer_height = height;

    init_render_target_view();
    init_depth_and_stencil_buffer();
}

void Renderer::bind_globals(const mat4& viewProj)
{
    view_projection_buffer.write(viewProj.data());
    Direct3D::globals().context4->VSSetConstantBuffers(0, 1, view_projection_buffer.address());
    Direct3D::globals().context4->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
}

void Renderer::clear_buffers(const float background_color[4])
{
    Direct3D::globals().context4->ClearRenderTargetView(target_view.Get(), background_color);
    Direct3D::globals().context4->ClearDepthStencilView(depth_stencil.view.Get(), D3D11_CLEAR_DEPTH, 0.f, 0);
}

void Renderer::prepare_output()
{
    Direct3D& globals = Direct3D::globals();

    D3D11_VIEWPORT viewport = {
        0.f, 0.f,
        FLOAT(scbuffer_width), FLOAT(scbuffer_height),
        0.f, 1.f
    };

    globals.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    globals.context4->RSSetViewports(1, &viewport);
    globals.context4->RSSetState(rasterizer_state.Get());
    globals.context4->OMSetDepthStencilState(depth_stencil.state.Get(), 1);
    globals.context4->OMSetRenderTargets(1, target_view.GetAddressOf(),depth_stencil.view.Get());
}
