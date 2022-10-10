#pragma once

#include "Direct11/DynamicBuffer.h"
#include "RandomGenerator.h"
#include "Timer.h"
#include "math/math.h"

struct Particle;
class ParticleSystem;

inline float smoke_particle_fading(float time, float lifetime)
{
	if (time < 0.5f)
		return 2.f * time;

	if (time > lifetime * 2.f / 3.f)
		return 3.f - 3.f * time / lifetime;

	return 1.f;
}


class SmokeEmitter
{
	friend ParticleSystem;

	vec3f position;
	Timer spawnTimer;
	float spawnRadius;

public:
	vec3f particleInitSize = {1.f, 1.f, 1.f},
		  particleInitTint = {1.f, 1.f, 1.f},
		  particleInitDirection = {0.f, 1.f, 0.f};
	float particleLifetime = 5.f,
		  particleSpeed = 4.f,
		  particleSizeScale = 1.002f;

	std::vector<Particle> particles;
	
	SmokeEmitter(const vec3f& position, float spawnRate, float spawnRadius):
		position(position), spawnTimer(spawnRate), spawnRadius(spawnRadius)
	{
		spawnTimer.start();
	}

	void update(float dt);
};

