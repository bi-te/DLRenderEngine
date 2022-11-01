#pragma once

#include "ShaderManager.h"
#include "Direct11/DynamicBuffer.h"
#include "SmokeEmitter.h"
#include "math/math.h"
#include "Direct11/Direct3D.h"

struct Particle
{
	vec3f position;
	vec3f direction;
	vec2f size;
	float thickness;
	float angle;
	float lifetime;
	vec4f tint;
};

struct ParticleBuffer
{	
	vec3f position;
	float angle;
	vec2f size;
	float thickness;
	vec2f tex_coor;
	float frameFracTime;
	vec2f tex_coor_next;
	vec3f tint;
	float alpha;
	vec2f image_size;
};

class ParticleSystem
{
	struct ParticleAtlas
	{
		uint32_t rows, columns;
		std::shared_ptr<Texture> smoke_emva1;
		std::shared_ptr<Texture> smoke_emission;
		std::shared_ptr<Texture> smoke_lightmap1;
		std::shared_ptr<Texture> smoke_lightmap2;
	};
	
	static ParticleSystem* s_system;
	ParticleSystem(){}

	ParticleSystem(const ParticleSystem& other) = delete;
	ParticleSystem(ParticleSystem&& other) noexcept = delete;
	ParticleSystem& operator=(const ParticleSystem& other) = delete;
	ParticleSystem& operator=(ParticleSystem&& other) noexcept = delete;

	DynamicBuffer instanceBuffer{D3D11_BIND_VERTEX_BUFFER};
public:
	ParticleAtlas atlas;
	std::vector<SmokeEmitter> emitters;
	std::vector<ParticleBuffer> pBuffer;
	std::shared_ptr<Shader> particle_shader;

	void add_smoke_emitter(const SmokeEmitter& emitter)
	{
		emitters.push_back(emitter);
	}

	void update(float dt);
	void updateInstanceBuffer(const Camera& camera);
	void render(const Camera& camera, comptr<ID3D11ShaderResourceView> depth_texture);

	static void init()
	{
		if (s_system) reset();

		s_system = new ParticleSystem;
	}

	static ParticleSystem& instance()
	{
		assert(s_system && "ParticleSystem not initialized");
		return *s_system;
	}

	static void reset()
	{
		s_system->particle_shader.reset();
		delete s_system;
	}
};

