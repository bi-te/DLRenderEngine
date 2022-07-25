#pragma once

#include "math/math.h"
#include "render/Direct11/DynamicBuffer.h"
#include "render/TextureManager.h"
#include "render/ShaderManager.h"

struct FrustrumBuffer
{
	vec4 bottom_left_point,
		 up_vector,
		 right_vector;
};

struct Skybox
{
	DynamicBuffer<D3D11_BIND_CONSTANT_BUFFER> frustrumBuffer{ sizeof(FrustrumBuffer) };
	uint32_t texture, shader;

	void update_frustrum_buffer(const FrustrumBuffer& frustrum)
	{
		frustrumBuffer.write(&frustrum);
	}

	void draw();
};

