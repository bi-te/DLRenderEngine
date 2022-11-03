#pragma once
#include <string>

#include "render/Direct11/RenderBuffer.h"
#include "render/Direct11/DynamicBuffer.h"

struct Shader;

struct PostProcessBuffer
{
	float ev100;
	uint32_t msaa;
	float padding[2];
};

class PostProcess
{
	DynamicBuffer postProcessBuffer{ D3D11_BIND_CONSTANT_BUFFER };
public:
	float ev100;
	std::shared_ptr<Shader> postProcessShader;
	std::shared_ptr<Shader> postProcessShaderMS;

	PostProcess()
	{
		postProcessBuffer.allocate(sizeof(PostProcessBuffer));
	}

	void update_buffer(uint32_t msaa);
	void resolve(RenderBuffer& hdrInput, RenderBuffer& ldrOutput);
	void resolve_msaa(RenderBuffer& hdrInput, RenderBuffer& ldrOutput);

	void render_reset()
	{
		postProcessBuffer.free();
	}
};

