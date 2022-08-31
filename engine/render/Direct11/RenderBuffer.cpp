#include "RenderBuffer.h"

const vec4f RenderBuffer::default_color { 0.f, 0.f, 0.f, 0.f };

void RenderBuffer::create(uint32_t buffer_width, uint32_t buffer_height, DXGI_FORMAT format, UINT bind_flags)
{
	width = buffer_width;
	height = buffer_height;

	comptr<ID3D11Texture2D> hdr;
	D3D11_TEXTURE2D_DESC desc{};
	desc.Width = buffer_width;
	desc.Height = buffer_height;
	desc.CPUAccessFlags = DXGI_CPU_ACCESS_NONE;
	desc.BindFlags = bind_flags;
	desc.Format = format;
	desc.MipLevels = 1u;
	desc.ArraySize = 1u;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.SampleDesc.Count = 1u;
	desc.SampleDesc.Quality = 0u;
	HRESULT result = Direct3D::instance().device5->CreateTexture2D(&desc, nullptr, &hdr);
	assert(SUCCEEDED(result) && "CreateTexture2D hdr texture");

	result = Direct3D::instance().device5->CreateRenderTargetView(hdr.Get(), nullptr, &rtv);
	assert(SUCCEEDED(result) && "HDR Texture as RendreTargetView");

	result = Direct3D::instance().device5->CreateShaderResourceView(hdr.Get(), nullptr, &srv);
	assert(SUCCEEDED(result) && "HDR Texture as ShaderResourceView");

}

void RenderBuffer::create(const D3D11_TEXTURE2D_DESC& desc)
{
	width = desc.Width;
	height = desc.Height;

	comptr<ID3D11Texture2D> hdr;
	HRESULT result = Direct3D::instance().device5->CreateTexture2D(&desc, nullptr, &hdr);
	assert(SUCCEEDED(result) && "CreateTexture2D hdr texture");

	result = Direct3D::instance().device5->CreateRenderTargetView(hdr.Get(), nullptr, &rtv);
	assert(SUCCEEDED(result) && "HDR Texture as RendreTargetView");

	result = Direct3D::instance().device5->CreateShaderResourceView(hdr.Get(), nullptr, &srv);
	assert(SUCCEEDED(result) && "HDR Texture as ShaderResourceView");
}

void RenderBuffer::bind_rtv(const comptr<ID3D11DepthStencilView>& dsView)
{
    D3D11_VIEWPORT viewport = { 0.f, 0.f,FLOAT(width), FLOAT(height),0.f, 1.f };

    Direct3D::instance().context4->OMSetRenderTargets(1, rtv.GetAddressOf(), dsView.Get());
    Direct3D::instance().context4->RSSetViewports(1, &viewport);
}

void RenderBuffer::bind_rtv()
{
    D3D11_VIEWPORT viewport = { 0.f, 0.f,FLOAT(width), FLOAT(height),0.f, 1.f };

    Direct3D::instance().context4->OMSetRenderTargets(1, rtv.GetAddressOf(), nullptr);
    Direct3D::instance().context4->RSSetViewports(1, &viewport);
}

void RenderBuffer::clear()
{
	Direct3D::instance().context4->ClearRenderTargetView(rtv.Get(), default_color.data());
}

void RenderBuffer::clear(const float color[4])
{
	Direct3D::instance().context4->ClearRenderTargetView(rtv.Get(), color);
}

void RenderBuffer::reset()
{
    rtv.Reset();
    srv.Reset();
}
