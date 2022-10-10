#include "SmokeEmitter.h"

#include "ParticleSystem.h"

void SmokeEmitter::update(float dt)
{
	if (spawnTimer.frame_time_check())
	{
		spawnTimer.advance_current();
		RandomGenerator& random = RandomGenerator::generator();

		float theta = random.get_real() * 2.f * PI;
		vec3f particlePosition = position;
		particlePosition.x() += spawnRadius * cosf(theta);
		particlePosition.z() += spawnRadius * sinf(theta);

		Particle new_particle{};
		new_particle.position = particlePosition;
		new_particle.angle = random.get_real() * 360.f;
		new_particle.direction = particleInitDirection;
		new_particle.size = particleInitSize.head<2>();
		new_particle.thickness = particleInitSize.z();
		new_particle.tint = vec4f{particleInitTint.x(), particleInitTint.y(), particleInitTint.z(), 0.f};

		particles.push_back(std::move(new_particle));
	}

	for (uint32_t i = 0; i < particles.size(); ++i)
	{
		Particle& particle = particles[i];
		particle.position += dt * particle.direction * particleSpeed;
		particle.size *= particleSizeScale;
		particle.lifetime += dt;
		if (particle.lifetime > particleLifetime)
			particles.erase(particles.begin() + i);
		else
			particle.tint(3) = smoke_particle_fading(particle.lifetime, particleLifetime);
	}
}
