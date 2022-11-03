#pragma once

#include "d3d.h"

enum TextureType { TextureDiffuse, TextureNormals, TextureMetallic, TextureRoughness };

struct Texture {
	comptr<ID3D11ShaderResourceView> srv;
	comptr<ID3D11RenderTargetView> rtv;

	void reset() {
		srv.Reset();
		rtv.Reset();
	}
};