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

void ParticleSystem::update_dissolving_particles(comptr<ID3D11ShaderResourceView> normals, comptr<ID3D11ShaderResourceView> depth)
{
	Direct3D& direct = Direct3D::instance();

	ID3D11UnorderedAccessView* uavs[3] = {
	dissolveParticleBuffer.rangeUAV.Get(),
	dissolveParticleBuffer.uav.Get(),
	dissolveParticleBuffer.indirectArgsUAV.Get()
	};

	ID3D11UnorderedAccessView* null_uavs[3] = {
		NULL_UAV,
		NULL_UAV,
		NULL_UAV
	};

	uint32_t offsets[3] = { -1, -1, -1 };

	direct.context4->CSSetConstantBuffers(1, 1, dissolveParticleBuffer.sizeBuffer.address());
	direct.context4->CSSetShaderResources(5, 1, normals.GetAddressOf());
	direct.context4->CSSetShaderResources(6, 1, depth.GetAddressOf());
	dissolutionParticlesUpdateShader->bind();
	direct.context4->CSSetUnorderedAccessViews(0, 3, uavs, offsets);
	direct.context4->Dispatch((dissolveParticleBuffer.size / 64u) + 1u, 1, 1);

	dissolutionParticlesArgsUpdateShader->bind();
	direct.context4->CSSetUnorderedAccessViews(0, 3, uavs, offsets);
	direct.context4->Dispatch(1, 1, 1);

	direct.context4->CSSetUnorderedAccessViews(0, 3, null_uavs, offsets);
}

void ParticleSystem::render(const Camera& camera, comptr<ID3D11ShaderResourceView> normals, comptr<ID3D11ShaderResourceView> depth)
{
	Direct3D& direct = Direct3D::instance();

	updateInstanceBuffer(camera);

	uint32_t istrides = sizeof(ParticleBuffer), ioffset = 0;
	direct.context4->IASetVertexBuffers(0, 1, &NULL_BUFFER, &istrides, &ioffset);
	direct.context4->IASetVertexBuffers(1, 1, instanceBuffer.address(), &istrides, &ioffset);
	direct.context4->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	particle_shader->bind();
	direct.context4->PSSetShaderResources(5, 1, atlas.smoke_emva1->srv.GetAddressOf());
	direct.context4->PSSetShaderResources(6, 1, atlas.smoke_emission->srv.GetAddressOf());
	direct.context4->PSSetShaderResources(7, 1, atlas.smoke_lightmap1->srv.GetAddressOf());
	direct.context4->PSSetShaderResources(8, 1, atlas.smoke_lightmap2->srv.GetAddressOf()); 
	direct.context4->PSSetShaderResources(9, 1, depth.GetAddressOf());

	direct.context4->DrawInstanced(6u, pBuffer.size(), 0u, 0u);

	update_dissolving_particles(normals, depth);
	dissolveParticleShader->bind();
	direct.context4->VSSetConstantBuffers(1, 1, dissolveParticleBuffer.sizeBuffer.address());
	direct.context4->VSSetShaderResources(0, 1, dissolveParticleBuffer.rangeSRV.GetAddressOf());
	direct.context4->VSSetShaderResources(1, 1, dissolveParticleBuffer.srv.GetAddressOf());
	direct.context4->DrawInstancedIndirect(dissolveParticleBuffer.indirectArgs.Get(), 0);
	direct.context4->VSSetShaderResources(0, 1, &NULL_SRV);
	direct.context4->VSSetShaderResources(1, 1, &NULL_SRV);
}

void ParticleSystem::init_dissolve_buffers(uint32_t size)
{
	Direct3D& direct = Direct3D::instance();
	HRESULT result;

	dissolveParticleBuffer.size = size;
	dissolveParticleBuffer.sizeBuffer.write(&size, sizeof(float[4]));

	// Particles
	D3D11_BUFFER_DESC bDesc{};
	bDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bDesc.ByteWidth = sizeof(DissolveParticle) * size;
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bDesc.StructureByteStride = sizeof(DissolveParticle);
	result = direct.device5->CreateBuffer(&bDesc, NULL, &dissolveParticleBuffer.particles);
	assert(SUCCEEDED(result) && "CreateBuffer UnorderedAccess DissolveParticleBuffer Particles");

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0u;
	uavDesc.Buffer.NumElements = size;
	uavDesc.Buffer.Flags = 0u;
	result = direct.device5->CreateUnorderedAccessView(dissolveParticleBuffer.particles.Get(), &uavDesc, &dissolveParticleBuffer.uav);
	assert(SUCCEEDED(result) && "CreateUnorderedAccessView DissolveParticleBuffer Particles");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0u;
	srvDesc.Buffer.NumElements = size;
	result = direct.device5->CreateShaderResourceView(dissolveParticleBuffer.particles.Get(), &srvDesc, &dissolveParticleBuffer.srv);

	// Range
	bDesc = {};
	bDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bDesc.ByteWidth = sizeof(uint32_t) * 3u;
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA subdata{};
	uint32_t zeros[3] = { 0u, 0u, 0u};
	subdata.pSysMem = &zeros;
	result = direct.device5->CreateBuffer(&bDesc, &subdata, &dissolveParticleBuffer.range);
	assert(SUCCEEDED(result) && "CreateBuffer UnorderedAccess DissolveParticleBuffer ParticlesRange");

	uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32_UINT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0u;
	uavDesc.Buffer.NumElements = 3u;
	uavDesc.Buffer.Flags = 0u;
	result = direct.device5->CreateUnorderedAccessView(dissolveParticleBuffer.range.Get(), &uavDesc, &dissolveParticleBuffer.rangeUAV);
	assert(SUCCEEDED(result) && "CreateUnorderedAccessView");

	srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0u;
	srvDesc.Buffer.NumElements = 3u;
	result = direct.device5->CreateShaderResourceView(dissolveParticleBuffer.range.Get(), &srvDesc, &dissolveParticleBuffer.rangeSRV);
	assert(SUCCEEDED(result) && "CreateUnorderedAccessView");

	// IndirectArgs
	bDesc = {};
	bDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	bDesc.ByteWidth = sizeof(uint32_t) * 4u;
	bDesc.Usage = D3D11_USAGE_DEFAULT;
	bDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	result = direct.device5->CreateBuffer(&bDesc, NULL, &dissolveParticleBuffer.indirectArgs);
	assert(SUCCEEDED(result) && "CreateBuffer UnorderedAccess DissolveParticleBuffer ParticlesIndirectArgs");

	uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_R32_UINT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0u;
	uavDesc.Buffer.NumElements = 4u;
	uavDesc.Buffer.Flags = 0u;
	result = direct.device5->CreateUnorderedAccessView(dissolveParticleBuffer.indirectArgs.Get(), &uavDesc, &dissolveParticleBuffer.indirectArgsUAV);
	assert(SUCCEEDED(result) && "CreateUnorderedAccessView");
}
