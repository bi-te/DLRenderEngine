#include "ParticleSystem.h"

ParticleSystem* ParticleSystem::s_system = nullptr;

void ParticleSystem::update(float dt)
{
	for (auto& emitter : emitters)
		emitter.update(dt);
}

void ParticleSystem::updateInstanceBuffer(const Camera& camera)
{
	uint32_t particleCount = 0u,
		particleCopied = 0u;

	for (auto& emitter : emitters)
		particleCount += emitter.particles.size();

	pBuffer.resize(particleCount);

	uint32_t frames = atlas.columns * atlas.rows;
	for (auto& emitter : emitters)
	{
		for (uint32_t particleInd = 0u; particleInd < emitter.particles.size(); particleInd++, particleCopied++)
		{
			Particle& particle = emitter.particles[particleInd];
			uint32_t frame = std::floorf(particle.lifetime * frames / emitter.particleLifetime );
			float frameTime = particle.lifetime / (emitter.particleLifetime / frames);

			pBuffer[particleCopied].tex_coor = { float(frame % atlas.columns) / atlas.columns, float(frame / atlas.rows) / atlas.rows};
			if(frame < frames)
			{
				frame++;
				pBuffer[particleCopied].tex_coor_next =  { float(frame % atlas.columns) / atlas.columns, float(frame / atlas.rows) / atlas.rows};
			}else
			{
				pBuffer[particleCopied].tex_coor_next = pBuffer[particleCopied].tex_coor;
			}
			pBuffer[particleCopied].image_size = {1.f / atlas.columns, 1.f / atlas.rows};
			pBuffer[particleCopied].frameFracTime = frameTime - std::floorf(frameTime);
			pBuffer[particleCopied].size = particle.size;
			pBuffer[particleCopied].thickness = particle.thickness;
			pBuffer[particleCopied].angle = particle.angle;
			pBuffer[particleCopied].position = particle.position;
			pBuffer[particleCopied].tint = {particle.tint.x(), particle.tint.y(), particle.tint.z()};
			pBuffer[particleCopied].alpha = particle.tint.w();
		}
	}

	std::sort(
		pBuffer.begin(), pBuffer.end(),
		[&camera](const ParticleBuffer& p1, const ParticleBuffer& p2)
		{
			const vec3f cam_view_z = camera.view.col(2).head<3>();
			return p1.position.dot(cam_view_z) > p2.position.dot(cam_view_z);
		}
	);

	instanceBuffer.write(pBuffer.data(), particleCount * sizeof(ParticleBuffer));
}

void ParticleSystem::render(const Camera& camera, comptr<ID3D11ShaderResourceView> depth_texture)
{
	Direct3D& direct = Direct3D::instance();

	updateInstanceBuffer(camera);

	uint32_t instance_stride = sizeof(ParticleBuffer), ioffset = 0;
	direct.context4->IASetVertexBuffers(0, 1, &NULL_BUFFER, &instance_stride, &ioffset);
	direct.context4->IASetVertexBuffers(1, 1, instanceBuffer.address(), &instance_stride, &ioffset);
	direct.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	particle_shader->bind();
	direct.context4->PSSetShaderResources(5, 1, atlas.smoke_emva1.GetAddressOf());
	direct.context4->PSSetShaderResources(6, 1, atlas.smoke_emission.GetAddressOf());
	direct.context4->PSSetShaderResources(7, 1, atlas.smoke_lightmap1.GetAddressOf());
	direct.context4->PSSetShaderResources(8, 1, atlas.smoke_lightmap2.GetAddressOf());
	direct.context4->PSSetShaderResources(9, 1, depth_texture.GetAddressOf());

	direct.context4->DrawInstanced(6u, pBuffer.size(), 0u, 0u);
}
