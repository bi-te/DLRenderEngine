#include "Direct3D.h"

#include <cstdint>

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

void Direct3D::init_rasterizer_state()
{
    D3D11_RASTERIZER_DESC rasterizer{};
    rasterizer.FillMode = D3D11_FILL_SOLID;
    rasterizer.CullMode = D3D11_CULL_BACK;
    rasterizer.DepthClipEnable = true;
    rasterizer.FrontCounterClockwise = false;
    HRESULT result = device5->CreateRasterizerState(&rasterizer, &rasterizer_state);
    assert(SUCCEEDED(result) && "CreateRasterizerState");
}

void Direct3D::init_sampler_state(D3D11_FILTER filter, uint8_t anisotropy)
{
    D3D11_SAMPLER_DESC sdesc{};
    sdesc.Filter = filter;
    sdesc.MaxAnisotropy = anisotropy;
    sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sdesc.MinLOD = 0.f;
    sdesc.MaxLOD = 10.f;
    device5->CreateSamplerState(&sdesc, &sampler_state);
}

void Direct3D::bind_globals(const PerFrame& per_frame_data)
{
    per_frame_buffer.write(&per_frame_data, context4);
    context4->VSSetConstantBuffers(0, 1, per_frame_buffer.address());
    context4->PSSetSamplers(0, 1, sampler_state.GetAddressOf());
}

void Direct3D::reset()
{
    if (!direct3d) return;

    direct3d->per_frame_buffer.free();

    direct3d->rasterizer_state.Reset();
    direct3d->sampler_state.Reset();

    direct3d->context4->ClearState();
    direct3d->context4->Flush();

    direct3d->factory5.Reset();
    direct3d->device5.Reset();
    direct3d->context4.Reset();

    direct3d->devdebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_SUMMARY | D3D11_RLDO_IGNORE_INTERNAL);
    direct3d->devdebug.Reset();

    delete direct3d;
}
