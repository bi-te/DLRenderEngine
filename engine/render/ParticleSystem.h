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

struct DissolveParticle {
	vec3f position;
	float spawnTime;
	vec3f dir;
	float alpha;
	vec3f size;
	vec3f color;
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

	void updateInstanceBuffer(const Camera& camera);
	void update_dissolving_particles(comptr<ID3D11ShaderResourceView> normals, comptr<ID3D11ShaderResourceView> depth);
public:
	struct DissolveParticleBuffers {
		uint32_t size;
		DynamicBuffer sizeBuffer{ D3D11_BIND_CONSTANT_BUFFER };
		comptr<ID3D11Buffer> particles;
		comptr<ID3D11UnorderedAccessView> uav;
		comptr<ID3D11ShaderResourceView> srv;
		comptr<ID3D11Buffer> range;
		comptr<ID3D11UnorderedAccessView> rangeUAV;
		comptr<ID3D11ShaderResourceView> rangeSRV;
		comptr<ID3D11Buffer> indirectArgs;
		comptr<ID3D11UnorderedAccessView> indirectArgsUAV;
	};
	DissolveParticleBuffers dissolveParticleBuffer;

	ParticleAtlas atlas;
	std::vector<SmokeEmitter> emitters;
	std::vector<ParticleBuffer> pBuffer;
	std::shared_ptr<Shader> particle_shader;
	std::shared_ptr<Shader> dissolveParticleShader;
	std::shared_ptr<ComputeShader> dissolutionParticlesUpdateShader;
	std::shared_ptr<ComputeShader> dissolutionParticlesArgsUpdateShader;

	void add_smoke_emitter(const SmokeEmitter& emitter)
	{
		emitters.push_back(emitter);
	}

	void update(float dt);
	void render(const Camera& camera, comptr<ID3D11ShaderResourceView> normals, comptr<ID3D11ShaderResourceView> depth);

	void init_dissolve_buffers(uint32_t size);

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

