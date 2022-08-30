#pragma once

#include "math/math.h"
#include "render/Direct11/d3d.h"

struct Shader;

struct Skybox
{
	comptr<ID3D11ShaderResourceView> texture;
	std::shared_ptr<Shader> skyshader;
	void render();
};

