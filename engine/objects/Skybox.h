#pragma once

#include "math/math.h"
#include "render/Direct11/d3d.h"
#include "render/Direct11/Texture.h"

struct Shader;

class Skybox
{
	struct Reflection
	{
		std::shared_ptr<Texture> map;
		uint32_t mip_levels;
	};
public:
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Texture> irradiance_map;
	Reflection reflection;
	std::shared_ptr<Shader> skyshader;

	void load_reflection(LPCWSTR file);
	void render();

	void reset()
	{
		texture.reset();
		irradiance_map.reset();
		reflection.map.reset();
	}
};

