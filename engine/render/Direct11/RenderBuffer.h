#pragma once

#include "Direct3D.h"

struct RenderBuffer
{
	static const vec4f default_color;

	uint32_t width, height;
	uint32_t msaa;
	comptr<ID3D11RenderTargetView> rtv;
	comptr<ID3D11ShaderResourceView> srv;

	void create(uint32_t buffer_width, uint32_t buffer_height, DXGI_FORMAT format, uint32_t buffer_msaa = 1, UINT bind_flags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	void create(const D3D11_TEXTURE2D_DESC& desc);

	void bind_rtv(const comptr<ID3D11DepthStencilView>& dsView);
	void bind_rtv();

	void clear();
	void clear(const float color[4]);

	void reset();
};