#pragma once

#include "math/math.h"
#include "render/Direct11/d3d.h"

struct Shader;

class Skybox
{
	struct Reflection
	{
		comptr<ID3D11ShaderResourceView> map;
		uint32_t mip_levels;
	};
public:
	comptr<ID3D11ShaderResourceView> texture;
	comptr<ID3D11ShaderResourceView> irradiance_map;
	Reflection reflection;
	std::shared_ptr<Shader> skyshader;

	void load_reflection(LPCWSTR file);
	void render();

	void reset()
	{
		texture.Reset();
		irradiance_map.Reset();
		reflection.map.Reset();
	}
};

