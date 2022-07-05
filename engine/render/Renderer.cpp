#include "Renderer.h"

void Renderer::init_swap_chain(HWND window)
{
    Direct3D& globals = Direct3D::globals();
    DXGI_SWAP_CHAIN_DESC1 desc;
    ZeroMemory(&desc, sizeof(desc));

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

    comptr<ID3D11Texture2D> back_buffer;
    result = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), &back_buffer);
    assert(SUCCEEDED(result) && "GetBuffer");

    result = globals.device5->CreateRenderTargetView(back_buffer.Get(), NULL, &target_view);
    assert(SUCCEEDED(result) && "CreateRenderTargetView");
}

void Renderer::update_vertex_shader(LPCWSTR file_name, LPCSTR entry_point)
{
    HRESULT result;

    result = D3DCompileFromFile(file_name, nullptr, nullptr, entry_point, "vs_5_0",
        D3DCOMPILE_DEBUG, NULL, &vertex_shader_blob, &error_blob);
    if (FAILED(result))
    {
        OutputDebugStringA((char*)error_blob->GetBufferPointer());
        assert(false && "VertexShader Compilation");
    }
    result = Direct3D::globals().device5->CreateVertexShader(vertex_shader_blob->GetBufferPointer(),
        vertex_shader_blob->GetBufferSize(), nullptr, &vertex_shader);
    assert(SUCCEEDED(result) && "CreateVertexShader");

    Direct3D::globals().context4->VSSetShader(vertex_shader.Get(), nullptr, 0);
}

void Renderer::update_pixel_shader(LPCWSTR file_name, LPCSTR entry_point)
{
    HRESULT result;

    result = D3DCompileFromFile(file_name, nullptr, nullptr, entry_point, "ps_5_0",
        D3DCOMPILE_DEBUG, NULL, &pixel_shader_blob, &error_blob);
    if (FAILED(result))
    {
        OutputDebugStringA((char*)error_blob->GetBufferPointer());
        assert(false && "PixelShader Compilation");
    }
    result = Direct3D::globals().device5->CreatePixelShader(pixel_shader_blob->GetBufferPointer(),
        pixel_shader_blob->GetBufferSize(), nullptr, &pixel_shader);
    assert(SUCCEEDED(result) && "CreateVertexShader");

    Direct3D::globals().context4->PSSetShader(pixel_shader.Get(), nullptr, 0);
}

void Renderer::create_input_layout()
{
    D3D11_INPUT_ELEMENT_DESC input_element_desc[2]
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            0, D3D11_INPUT_PER_VERTEX_DATA , 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,0,
            D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    HRESULT result = Direct3D::globals().device5->CreateInputLayout(input_element_desc,
        ARRAYSIZE(input_element_desc), vertex_shader_blob->GetBufferPointer(),
        vertex_shader_blob->GetBufferSize(), &layout);
    assert(SUCCEEDED(result) && "CreateInputLayout");
}

void Renderer::draw(Scene& scene)
{
    Direct3D& globals = Direct3D::globals();
    DXGI_SWAP_CHAIN_DESC desc;
    swap_chain->GetDesc(&desc);

    float backgroundColor[4] = { AMBIENT.x(), AMBIENT.y(), AMBIENT.z(), 1.f };
    globals.context4->ClearRenderTargetView(target_view.Get(), backgroundColor);

    D3D11_VIEWPORT viewport = { 0.f, 0.f,
        FLOAT(desc.BufferDesc.Width),
        FLOAT(desc.BufferDesc.Height),
        0.f, 1.f
    };

    globals.context4->RSSetViewports(1, &viewport);
    globals.context4->OMSetRenderTargets(1, target_view.GetAddressOf(), NULL);
    globals.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    globals.context4->IASetInputLayout(layout.Get());
    globals.context4->IASetVertexBuffers(0, 1, scene.vertexBuffer.GetAddressOf(),
        &scene.vertex_buffer_stride, &scene.vertex_buffer_offset);

    globals.context4->Draw(3, 0);
    swap_chain->Present(1, 0);
}
