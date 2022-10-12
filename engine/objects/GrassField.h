#pragma once

#include "math/poisson.h"
#include "render/ShaderManager.h"

#include "render/Direct11/Direct3D.h"
#include "render/Direct11/DynamicBuffer.h"

struct GrassInstanceBuffer {
	vec3f position;
	vec2f rel_pos;
};

struct GrassBuffer
{
	uint32_t planes;
	uint32_t sectors;
	vec2f scale;
};

class GrassField
{
	DynamicBuffer instanceBuffer{ D3D11_BIND_VERTEX_BUFFER };
	DynamicBuffer constantGrassBuffer{ D3D11_BIND_CONSTANT_BUFFER };

	vec3f world_position;
	float width, height;
	std::vector<std::array<float, 2>> points;

public:
	GrassBuffer grassBuffer;

	std::shared_ptr<Shader> grassShader;
	std::shared_ptr<Shader> pointShadowShader;
	std::shared_ptr<Shader> spotShadowShader;
	comptr<ID3D11ShaderResourceView> baseColor, normal, roughness, opacity, translucency, ambient_occlusion;

	void init_field(vec3f world_offset, float field_width, float field_height, float object_radius);
	void update_instance_buffer();
	void render();
	void shadow_render(uint32_t light_count);

	void reset()
	{
		baseColor.Reset();
		normal.Reset();
		roughness.Reset();
		opacity.Reset();
		translucency.Reset();
		ambient_occlusion.Reset();
		instanceBuffer.free();
		constantGrassBuffer.free();
	}
};

