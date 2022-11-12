#include "Direct3D.h"

#include <cstdint>

#include "Engine.h"
#include "EngineClock.h"
#include "render/LightSystem.h"

extern "C"
{
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

Direct3D* Direct3D::direct3d;

void Direct3D::init_core()
{
    HRESULT result;
    result = CreateDXGIFactory(__uuidof(IDXGIFactory5), &factory5);
    assert(result >= 0 && "CreateDXGIFactory");

    comptr<ID3D11Device> device;
    comptr<ID3D11DeviceContext> context;

    const D3D_FEATURE_LEVEL featureLevelRequested = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL featureLevelInitialized = D3D_FEATURE_LEVEL_11_0;
    result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG,
        &featureLevelRequested, 1, D3D11_SDK_VERSION, &device, &featureLevelInitialized, &context);
    assert(result >= 0 && "D3D11CreateDevice");
    assert(featureLevelInitialized == featureLevelRequested && "D3D_FEATURE_LEVEL_11_0");

    result = device->QueryInterface(__uuidof(ID3D11Device5), &device5);
    assert(result >= 0 && "Query ID3D11Device5");

    result = device->QueryInterface(__uuidof(ID3D11Debug), &devdebug);
    assert(result >= 0 && "Query ID3D11Debug");

    result = context->QueryInterface(__uuidof(ID3D11DeviceContext4), &context4);
    assert(result >= 0 && "Query ID3D11DeviceContext4");
}

void Direct3D::init_rasterizer_states()
{
    HRESULT result;
    D3D11_RASTERIZER_DESC rasterizer{};
    rasterizer.FillMode = D3D11_FILL_SOLID;
    rasterizer.CullMode = D3D11_CULL_BACK;
    rasterizer.DepthClipEnable = true;
    rasterizer.FrontCounterClockwise = false;
    rasterizer.MultisampleEnable = true;
    result = device5->CreateRasterizerState(&rasterizer, &rasterizer_state);
    assert(SUCCEEDED(result) && "CreateRasterizerState");

	D3D11_RASTERIZER_DESC two_face{};
    two_face.FillMode = D3D11_FILL_SOLID;
    two_face.CullMode = D3D11_CULL_NONE;
    two_face.DepthClipEnable = true;
    two_face.FrontCounterClockwise = false;
    two_face.MultisampleEnable = true;
    result = device5->CreateRasterizerState(&two_face, &two_face_rasterizer_state);
    assert(SUCCEEDED(result) && "CreateRasterizerState Two-Face");

    D3D11_RASTERIZER_DESC back_face{};
    back_face.FillMode = D3D11_FILL_SOLID;
    back_face.CullMode = D3D11_CULL_FRONT;
    back_face.DepthClipEnable = true;
    back_face.FrontCounterClockwise = false;
    back_face.MultisampleEnable = true;
    result = device5->CreateRasterizerState(&back_face, &back_face_rasterizer_state);
    assert(SUCCEEDED(result) && "CreateRasterizerState Back-Face");
}

void Direct3D::init_blend_state()
{
    HRESULT result;

    D3D11_BLEND_DESC blend_desc{};
    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;
    blend_desc.RenderTarget[0].BlendEnable = TRUE;
    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device5->CreateBlendState(&blend_desc, &blend_state);
    assert(SUCCEEDED(result) && "CreateBlendState");

    blend_desc.AlphaToCoverageEnable = FALSE;
    blend_desc.IndependentBlendEnable = FALSE;
    blend_desc.RenderTarget[0].BlendEnable = TRUE;
    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device5->CreateBlendState(&blend_desc, &additive_blend_state);
    assert(SUCCEEDED(result) && "CreateBlendState Additive");
}

void Direct3D::init_alpha_to_coverage()
{
    D3D11_BLEND_DESC alpha_to_coverage{};
    alpha_to_coverage.AlphaToCoverageEnable = TRUE;
    alpha_to_coverage.IndependentBlendEnable = FALSE;
    alpha_to_coverage.RenderTarget[0].BlendEnable = FALSE;
    alpha_to_coverage.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    HRESULT result = device5->CreateBlendState(&alpha_to_coverage, &atc_blend_state);
    assert(SUCCEEDED(result) && "CreateBlendState");
}

void Direct3D::init_sampler_state(D3D11_FILTER filter, uint8_t anisotropy)
{
    D3D11_SAMPLER_DESC sdesc{};
    sdesc.Filter = filter;
    sdesc.MaxAnisotropy = anisotropy;
    sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.MinLOD = 0.f;
    sdesc.MaxLOD = 12.f;
    device5->CreateSamplerState(&sdesc, &sampler_state);
}

void Direct3D::init_linear_clamp_sampler()
{
    D3D11_SAMPLER_DESC sdesc{};
    sdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sdesc.MinLOD = 0.f;
    sdesc.MaxLOD = 12.f;
    device5->CreateSamplerState(&sdesc, &linear_clamp_sampler_state);
}

void Direct3D::init_comparison_sampler()
{
    D3D11_SAMPLER_DESC sdesc{};
    sdesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sdesc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
    sdesc.MinLOD = 0.f;
    sdesc.MaxLOD = 12.f;
    device5->CreateSamplerState(&sdesc, &comparison_sampler_state);
}

void Direct3D::bind_globals(const Camera& camera, uint32_t max_reflection_mip)
{
    PerFrame* per_frame = static_cast<PerFrame*>(per_frame_buffer.map().pData);

    per_frame->view_projection = camera.view_proj;
    per_frame->frustum.bottom_left_point = camera.blnear_fpoint - camera.view_inv.row(3);
    per_frame->frustum.up_vector = camera.frustrum_up;
    per_frame->frustum.right_vector = camera.frustrum_right;
    per_frame->camera_pos = camera.position();
    per_frame->max_reflection_mip = max_reflection_mip;
    per_frame->near = camera.zn;
    per_frame->far = camera.zf;
    per_frame->time = EngineClock::instance().nowf();
    per_frame->frame_time = Engine::instance().applicationTimer.get_last_check_time();

    LightSystem::instance().bind_lights(&per_frame->light_buffer);

    per_frame_buffer.unmap();
    context4->VSSetConstantBuffers(0, 1, per_frame_buffer.address());
    context4->GSSetConstantBuffers(0, 1, per_frame_buffer.address());
    context4->PSSetConstantBuffers(0, 1, per_frame_buffer.address());
    context4->CSSetConstantBuffers(0, 1, per_frame_buffer.address());
    context4->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
    context4->PSSetSamplers(1, 1, linear_clamp_sampler_state.GetAddressOf());
    context4->PSSetSamplers(2, 1, comparison_sampler_state.GetAddressOf());
    context4->CSSetSamplers(0, 1, sampler_state.GetAddressOf());
    context4->CSSetSamplers(1, 1, linear_clamp_sampler_state.GetAddressOf());
    context4->CSSetSamplers(2, 1, comparison_sampler_state.GetAddressOf());
    context4->PSSetShaderResources(0, 1, reflectance_map->srv.GetAddressOf());
}

void Direct3D::init() 
{
    if (direct3d) reset();

    direct3d = new Direct3D;
    direct3d->init_core();
    direct3d->init_rasterizer_states();
    direct3d->init_blend_state();
    direct3d->init_alpha_to_coverage();
    direct3d->init_sampler_state();
    direct3d->init_linear_clamp_sampler();
    direct3d->init_comparison_sampler();

    direct3d->per_frame_buffer.allocate(sizeof(PerFrame));
}

void Direct3D::reset()
{
    if (!direct3d) return;

    direct3d->per_frame_buffer.free();
    direct3d->resolve_buffer.free();

    direct3d->rasterizer_state.Reset();
    direct3d->two_face_rasterizer_state.Reset();
    direct3d->back_face_rasterizer_state.Reset();
    direct3d->additive_blend_state.Reset();
    direct3d->atc_blend_state.Reset();
    direct3d->blend_state.Reset();
    direct3d->sampler_state.Reset();
    direct3d->linear_clamp_sampler_state.Reset();
    direct3d->comparison_sampler_state.Reset();

    direct3d->reflectance_map.reset();

    direct3d->context4->ClearState();
    direct3d->context4->Flush();

    direct3d->factory5.Reset();
    direct3d->device5.Reset();
    direct3d->context4.Reset();

    direct3d->devdebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_SUMMARY | D3D11_RLDO_IGNORE_INTERNAL);
    direct3d->devdebug.Reset();

    delete direct3d;
}

void Direct3D::resolve_depth(comptr<ID3D11ShaderResourceView> msaa_depth, comptr<ID3D11DepthStencilView> target,
	uint32_t msaa)
{
    depth_resolve_shader->bind();

    ResolveBuffer rb = {msaa};
    resolve_buffer.write(&rb, sizeof(ResolveBuffer));

    context4->OMSetRenderTargets(1, &NULL_RTV, target.Get());
    context4->PSSetShaderResources(5, 1, msaa_depth.GetAddressOf());
    context4->PSSetConstantBuffers(1, 1, resolve_buffer.address());
    context4->Draw(3, 0);
    context4->PSSetShaderResources(5, 1, &NULL_SRV);
}
