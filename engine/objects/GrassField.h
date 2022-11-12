#pragma once

#include "render/ShaderManager.h"

#include "render/Direct11/Direct3D.h"
#include "render/Direct11/DynamicBuffer.h"
#include "render/Direct11/Texture.h"

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
	std::vector<vec2f> points;

public:
	GrassBuffer grassBuffer;

	std::shared_ptr<Shader> grassShader;
	std::shared_ptr<Shader> grassDeferredShader;
	std::shared_ptr<Shader> pointShadowShader;
	std::shared_ptr<Shader> spotShadowShader;
	std::shared_ptr<Texture> baseColor, normal, roughness, opacity, translucency, ambient_occlusion;

	void init_field(vec3f world_offset, float field_width, float field_height, float object_radius);
	void update_instance_buffer();
	void render(bool forward_rendering);
	void shadow_render(uint32_t light_count);

	void reset()
	{
		baseColor.reset();
		normal.reset();
		roughness.reset();
		opacity.reset();
		translucency.reset();
		ambient_occlusion.reset();
		instanceBuffer.free();
		constantGrassBuffer.free();
	}
};

