#pragma once
#include <vector>
#include <math/math.h>

#include "Direct11/DynamicBuffer.h"
#include "Direct11/Texture.h"
#include "ShaderManager.h"

struct Decal {
	mat3f decalToWorld;
	vec3f position;
	float roughness;
	vec3f size;
	float rotation_angle;
	vec3f color;
	uint32_t objectId;
};

class DecalSystem
{
	static DecalSystem* s_system;

	DecalSystem() {}

	DecalSystem(const DecalSystem& other) = delete;
	DecalSystem(DecalSystem&& other) noexcept = delete;
	DecalSystem& operator=(const DecalSystem& other) = delete;
	DecalSystem& operator=(DecalSystem&& other) noexcept = delete;

	DynamicBuffer instanceBuffer{ D3D11_BIND_VERTEX_BUFFER };
public:
	std::vector<Decal> decals;
	std::shared_ptr<Shader> decalShader;
	std::shared_ptr<Texture> decalTexture;

	void updateInstanceBuffer();
	void render(comptr<ID3D11ShaderResourceView> depth,
		comptr<ID3D11ShaderResourceView>normals,
		comptr<ID3D11ShaderResourceView> ids);

	static void init() {
		if (s_system) reset();
		s_system = new DecalSystem;
	}

	static DecalSystem& instance() {
		assert(s_system && "DecalSystem no tinitialized");
		return *s_system;
	}

	static void reset() {
		delete s_system;
	}
};

