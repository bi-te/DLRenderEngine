#include "Direct3D.h"

#include <cstdint>

extern "C"
{
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

Direct3D* Direct3D::direct3d;

Direct3D::Direct3D()
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
