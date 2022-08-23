#pragma once
#include <string>

#include "render/Direct11/RenderBuffer.h"
#include "render/Direct11/DynamicBuffer.h"

struct PostProcessBuffer
{
	float ev100;
	float padding[3];
};

class PostProcess
{
	DynamicBuffer postProcessBuffer{ D3D11_BIND_CONSTANT_BUFFER };
public:
	float ev100;
	std::wstring post_process_shader;

	PostProcess()
	{
		postProcessBuffer.allocate(sizeof(PostProcessBuffer));
	}

	void update();

	void resolve(RenderBuffer& hdrInput, RenderBuffer& ldrOutput) const;

	void render_reset()
	{
		postProcessBuffer.free();
	}
};

