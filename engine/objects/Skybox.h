#pragma once

#include "math/math.h"
#include "render/Direct11/d3d.h"

struct Shader;

struct Skybox
{
	comptr<ID3D11ShaderResourceView> texture;
	comptr<ID3D11ShaderResourceView> irradiance_map;
	comptr<ID3D11ShaderResourceView> reflection_map;
	comptr<ID3D11ShaderResourceView> reflectance_map;
	std::shared_ptr<Shader> skyshader;
	void render();

	void reset()
	{
		texture.Reset();
		irradiance_map.Reset();
		reflection_map.Reset();;
		reflectance_map.Reset();;
	}
};

