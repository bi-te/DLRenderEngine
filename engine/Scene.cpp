#include "Scene.h"

#include <iostream>

#include "PostProcess.h"
#include "imgui/imgui.h"
#include "imgui/ImGuiManager.h"
#include "render/LightSystem.h"
#include "render/MeshSystem.h"
#include "render/ParticleSystem.h"
#include "render/ShaderManager.h"
#include "render/Direct11/Direct3D.h"
#include "render/DecalSystem.h"
#include <render/ModelManager.h>

void Scene::init_depth_stencil_buffer(uint32_t width, uint32_t height)
{
	//comptr<ID3D11Texture2D> buffer;

	// MSAA
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDesc.SampleDesc.Count = hdr_buffer.msaa;
	depthBufferDesc.SampleDesc.Quality = 0;
	HRESULT result = Direct3D::instance().device5->CreateTexture2D(&depthBufferDesc, nullptr, &depth_stencil.msaa_buffer);
	assert(SUCCEEDED(result) && "CreateTexture2D Depth Stencil");

	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc{};
	depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthViewDesc.Texture2D.MipSlice = 0;

	result = Direct3D::instance().device5->CreateDepthStencilView(depth_stencil.msaa_buffer.Get(),
		&depthViewDesc, &depth_stencil.msaa_view);
	assert(SUCCEEDED(result) && "CreateDepthStencilView");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	result = Direct3D::instance().device5->CreateShaderResourceView(depth_stencil.msaa_buffer.Get(), &srvDesc, &depth_stencil.msaa_srv);
	assert(SUCCEEDED(result) && "Depth CreateShaderResourceView");

	// Non-MSAA
	depthBufferDesc = {};
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	result = Direct3D::instance().device5->CreateTexture2D(&depthBufferDesc, nullptr, &depth_stencil.buffer);
	assert(SUCCEEDED(result) && "CreateTexture2D Depth Stencil");

	depthViewDesc = {};
	depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Texture2D.MipSlice = 0;

	result = Direct3D::instance().device5->CreateDepthStencilView(depth_stencil.buffer.Get(),
		&depthViewDesc, &depth_stencil.view);
	assert(SUCCEEDED(result) && "CreateDepthStencilView");

	srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1u;
	result = Direct3D::instance().device5->CreateShaderResourceView(depth_stencil.buffer.Get(), &srvDesc, &depth_stencil.srv);
	assert(SUCCEEDED(result) && "Depth CreateShaderResourceView");
}

void Scene::init_depth_stencil_states()
{
	HRESULT result;

	D3D11_DEPTH_STENCIL_DESC depthDesc{};
	depthDesc.DepthEnable = TRUE;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	depthDesc.StencilEnable = TRUE;
	depthDesc.StencilReadMask = 0xFF;
	depthDesc.StencilWriteMask = 0xFF;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
	depthDesc.BackFace = depthDesc.FrontFace;

	result = Direct3D::instance().device5->CreateDepthStencilState(&depthDesc, &depth_stencil.state);
	assert(SUCCEEDED(result) && "CreateDepthStencilState");

	D3D11_DEPTH_STENCIL_DESC readOnlyDepth{};
	readOnlyDepth.DepthEnable = TRUE;
	readOnlyDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	readOnlyDepth.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	result = Direct3D::instance().device5->CreateDepthStencilState(&readOnlyDepth, &depth_stencil.read_only_state);
	assert(SUCCEEDED(result) && "CreateDepthStencilState");

	D3D11_DEPTH_STENCIL_DESC stencilTest{};
	stencilTest.DepthEnable = FALSE;
	stencilTest.StencilEnable = TRUE;
	stencilTest.StencilReadMask = 0xFF;
	stencilTest.StencilWriteMask = 0xFF;
	stencilTest.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	stencilTest.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	stencilTest.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	stencilTest.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	stencilTest.BackFace = stencilTest.FrontFace;

	result = Direct3D::instance().device5->CreateDepthStencilState(&stencilTest, &depth_stencil.stencil_test_state);
	assert(SUCCEEDED(result) && "CreateDepthStencilState");
}

void Scene::init_gBuffer(uint32_t width, uint32_t height) {
	HRESULT result;
	Direct3D& direct = Direct3D::instance();

	comptr<ID3D11Texture2D> gTexture;
	D3D11_TEXTURE2D_DESC gDesc{};
	gDesc.Width = width;
	gDesc.Height = height;
	gDesc.ArraySize = 1u;
	gDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	gDesc.MipLevels = 1u;
	gDesc.SampleDesc.Count = 1u;
	gDesc.SampleDesc.Quality = 0u;
	gDesc.Usage = D3D11_USAGE_DEFAULT;

	gDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	result = direct.device5->CreateTexture2D(&gDesc, nullptr, &gTexture);
	assert(SUCCEEDED(result) && "GBuffer Albedo CreateTexture2D");
	result = direct.device5->CreateShaderResourceView(gTexture.Get(), nullptr, &g_buffer.albedo.srv);
	assert(SUCCEEDED(result) && "GBuffer Albedo CreateShaderResourceView");
	result = direct.device5->CreateRenderTargetView(gTexture.Get(), nullptr, &g_buffer.albedo.rtv);
	assert(SUCCEEDED(result) && "GBuffer Albedo CreateRenderTargetView");

	gDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	result = direct.device5->CreateTexture2D(&gDesc, nullptr, &gTexture);
	assert(SUCCEEDED(result) && "GBuffer Roughness Metallness CreateTexture2D");
	result = direct.device5->CreateShaderResourceView(gTexture.Get(), nullptr, &g_buffer.roughMetalTransmittance.srv);
	assert(SUCCEEDED(result) && "GBuffer Roughness Metallness CreateShaderResourceView");
	result = direct.device5->CreateRenderTargetView(gTexture.Get(), nullptr, &g_buffer.roughMetalTransmittance.rtv);
	assert(SUCCEEDED(result) && "GBuffer Roughness Metallness CreateRenderTargetView");

	gDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	result = direct.device5->CreateTexture2D(&gDesc, nullptr, &gTexture);
	assert(SUCCEEDED(result) && "GBuffer Emission CreateTexture2D");
	result = direct.device5->CreateShaderResourceView(gTexture.Get(), nullptr, &g_buffer.emission.srv);
	assert(SUCCEEDED(result) && "GBuffer Emission CreateShaderResourceView");
	result = direct.device5->CreateRenderTargetView(gTexture.Get(), nullptr, &g_buffer.emission.rtv);
	assert(SUCCEEDED(result) && "GBuffer Emission CreateRenderTargetView");

	gDesc.Format = DXGI_FORMAT_R16_UINT;
	result = direct.device5->CreateTexture2D(&gDesc, nullptr, &gTexture);
	assert(SUCCEEDED(result) && "GBuffer Ids CreateTexture2D");
	result = direct.device5->CreateShaderResourceView(gTexture.Get(), nullptr, &g_buffer.id.srv);
	assert(SUCCEEDED(result) && "GBuffer Ids CreateShaderResourceView");
	result = direct.device5->CreateRenderTargetView(gTexture.Get(), nullptr, &g_buffer.id.rtv);
	assert(SUCCEEDED(result) && "GBuffer Ids CreateRenderTargetView");

	gDesc.Format = DXGI_FORMAT_R16G16B16A16_SNORM;
	result = direct.device5->CreateTexture2D(&gDesc, nullptr, &g_buffer.normalsTexture);
	assert(SUCCEEDED(result) && "GBuffer Normals CreateTexture2D");
	result = direct.device5->CreateShaderResourceView(g_buffer.normalsTexture.Get(), nullptr, &g_buffer.normals.srv);
	assert(SUCCEEDED(result) && "GBuffer Normals CreateShaderResourceView");
	result = direct.device5->CreateRenderTargetView(g_buffer.normalsTexture.Get(), nullptr, &g_buffer.normals.rtv);
	assert(SUCCEEDED(result) && "GBuffer Normals CreateRenderTargetView");
	result = direct.device5->CreateTexture2D(&gDesc, nullptr, &g_buffer.normalsTextureCopy);
	assert(SUCCEEDED(result) && "GBuffer NormalsCopy CreateTexture2D");
	result = direct.device5->CreateShaderResourceView(g_buffer.normalsTextureCopy.Get(), nullptr, &g_buffer.normalsCopy.srv);
	assert(SUCCEEDED(result) && "GBuffer NormalsCopy CreateShaderResourceView");

	g_buffer.rtvs = { g_buffer.emission.rtv.Get(),
			g_buffer.normals.rtv.Get(),
			g_buffer.albedo.rtv.Get(),
			g_buffer.roughMetalTransmittance.rtv.Get(),
			g_buffer.id.rtv.Get()
	};

	g_buffer.viewport = { 0, 0, FLOAT(width),FLOAT(height), 0u, 1u };

	init_depth_stencil_buffer(width, height);
}

void Scene::init_hdr_and_depth_buffer(uint32_t width, uint32_t height, uint32_t msaa)
{
	hdr_buffer.create(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, msaa);
}
void Scene::shadow_pass()
{
	MeshSystem& mesh_system = MeshSystem::instance();
	LightSystem& light_system = LightSystem::instance();

	light_system.bind_depth_state();

	if (light_system.plights().size())
	{
		light_system.bind_point_dsv();
		mesh_system.opaque_instances.pointShadowShader->bind();
		mesh_system.opaque_instances.shadow_render(light_system.plights().size());
		mesh_system.appearing_instances.pointShadowShader->bind();
		mesh_system.appearing_instances.shadow_render(light_system.plights().size());
		mesh_system.dissolution_instances.pointShadowShader->bind();
		mesh_system.dissolution_instances.shadow_render(light_system.plights().size());
		grassfield.pointShadowShader->bind();
		grassfield.shadow_render(light_system.plights().size());
	}

	if (light_system.slights().size())
	{
		light_system.bind_spot_dsv();
		mesh_system.opaque_instances.spotShadowShader->bind();
		mesh_system.opaque_instances.shadow_render(light_system.slights().size());
	}
}

void Scene::resolve_gBuffer()
{
	Direct3D& direct = Direct3D::instance();
	LightSystem& lights = LightSystem::instance();
	direct.context4->OMSetBlendState(direct.additive_blend_state.Get(), 0, 0xffffffff);
	direct.context4->OMSetDepthStencilState(depth_stencil.stencil_test_state.Get(), 1);
	direct.context4->PSSetShaderResources(5, 1, g_buffer.emission.srv.GetAddressOf());
	direct.context4->PSSetShaderResources(6, 1, g_buffer.normals.srv.GetAddressOf());
	direct.context4->PSSetShaderResources(7, 1, g_buffer.albedo.srv.GetAddressOf());
	direct.context4->PSSetShaderResources(8, 1, g_buffer.roughMetalTransmittance.srv.GetAddressOf());
	direct.context4->PSSetShaderResources(9, 1, depth_stencil.srv.GetAddressOf());

	deferredResolveShader->bind();
	direct.context4->Draw(3, 0);

	direct.context4->RSSetState(direct.back_face_rasterizer_state.Get());
	deferredResolvePointLightsShader->bind();
	lights.bind_instance_buffer();
	for (auto& mesh : lights.pointLightRenderVolume->meshes) {
		direct.context4->DrawIndexedInstanced(mesh.m_range.numIndices,
			lights.plights().size() + lights.dlights().size(), mesh.m_range.indicesOffset, mesh.m_range.verticesOffset, 0);
	}

	direct.context4->PSSetShaderResources(5, 1, &NULL_SRV);
	direct.context4->PSSetShaderResources(6, 1, &NULL_SRV);
	direct.context4->PSSetShaderResources(7, 1, &NULL_SRV);
	direct.context4->PSSetShaderResources(8, 1, &NULL_SRV);
	direct.context4->PSSetShaderResources(9, 1, &NULL_SRV);
}

void Scene::bind_srvs()
{
	Direct3D& direct = Direct3D::instance();

	direct.context4->PSSetShaderResources(1, 1, skybox.irradiance_map->srv.GetAddressOf());
	direct.context4->PSSetShaderResources(2, 1, skybox.reflection.map->srv.GetAddressOf());
	direct.context4->PSSetShaderResources(3, 1, LightSystem::instance().depthBuffer.point_srv.GetAddressOf());
	direct.context4->PSSetShaderResources(4, 1, LightSystem::instance().depthBuffer.spot_srv.GetAddressOf());
}

void Scene::render(RenderBuffer& target_buffer, PostProcess& post_process, const Camera& camera)
{
	Direct3D& direct = Direct3D::instance();
	MeshSystem& meshes = MeshSystem::instance();
	DecalSystem& decals = DecalSystem::instance();
	ParticleSystem& particle_system = ParticleSystem::instance();
	bool forward_rendering = false;

	if (forward_rendering) {
		// Forward rendering
		// Global bindings
		direct.bind_globals(camera, skybox.reflection.mip_levels - 1u);
		direct.context4->PSSetShaderResources(3, 1, &NULL_SRV);
		direct.context4->PSSetShaderResources(4, 1, &NULL_SRV);

		// Render shadow maps
		shadow_pass();

		// Render preparation
		hdr_buffer.clear();
		hdr_buffer.bind_rtv(depth_stencil.msaa_view);

		direct.context4->ClearDepthStencilView(depth_stencil.msaa_view.Get(), D3D11_CLEAR_DEPTH, 0.f, 0);
		direct.context4->ClearDepthStencilView(depth_stencil.view.Get(), D3D11_CLEAR_DEPTH, 0.f, 0u);
		direct.context4->RSSetState(direct.rasterizer_state.Get());
		direct.context4->OMSetDepthStencilState(depth_stencil.state.Get(), 1);
		direct.context4->OMSetBlendState(nullptr, 0, 0xffffffff);

		// Render opaque & emissive objects 
		bind_srvs();
		meshes.render(forward_rendering);
		skybox.render();

		// Masked objects
		direct.context4->OMSetBlendState(direct.atc_blend_state.Get(), 0, 0xffffffff);
		grassfield.render(forward_rendering);

		// Resolving msaa depth buffer before drawing appearing instances
		direct.resolve_depth(depth_stencil.msaa_srv, depth_stencil.view, hdr_buffer.msaa);

		// Appearing instances
		hdr_buffer.bind_rtv(depth_stencil.msaa_view);
		meshes.appearing_instances.render(forward_rendering);

		// Render semi-transparent objects;
		direct.context4->OMSetDepthStencilState(depth_stencil.read_only_state.Get(), 1);
		direct.context4->OMSetBlendState(direct.blend_state.Get(), 0, 0xffffffff);
		particle_system.render(camera, g_buffer.normals.srv, depth_stencil.srv);

		// Resolving hdr-buffer to output
		post_process.resolve(hdr_buffer, target_buffer);
		direct.context4->PSSetShaderResources(0, 1, &NULL_SRV);

		if (ImGuiManager::active())
			ImGuiManager::flush();
	}
	else {
		//Deffered rendering

		direct.bind_globals(camera, skybox.reflection.mip_levels - 1);
		direct.context4->PSSetShaderResources(3, 1, &NULL_SRV);
		direct.context4->PSSetShaderResources(4, 1, &NULL_SRV);

		hdr_buffer.clear();
		g_buffer.clear();
		direct.context4->ClearDepthStencilView(depth_stencil.msaa_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.f, 0u);
		direct.context4->ClearDepthStencilView(depth_stencil.view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.f, 0u);

		direct.context4->RSSetState(direct.rasterizer_state.Get());
		direct.context4->OMSetBlendState(nullptr, 0, 0xffffffff);

		shadow_pass();

		direct.context4->OMSetDepthStencilState(depth_stencil.state.Get(), 1u);
		g_buffer.bind(depth_stencil.msaa_view);
		meshes.render(forward_rendering);
		meshes.appearing_instances.render(forward_rendering);
		meshes.dissolution_instances.render(forward_rendering);
		grassfield.render(forward_rendering);

		direct.context4->CopyResource(g_buffer.normalsTextureCopy.Get(), g_buffer.normalsTexture.Get());
		direct.context4->CopyResource(depth_stencil.buffer.Get(), depth_stencil.msaa_buffer.Get());

		meshes.dissolution_instances.spawn_particles(ParticleSystem::instance().dissolveParticleBuffer);

		direct.context4->OMSetRenderTargets(4, g_buffer.rtvs.data(), nullptr);
		decals.render(depth_stencil.srv, g_buffer.normalsCopy.srv, g_buffer.id.srv);

		hdr_buffer.bind_rtv(depth_stencil.msaa_view);
		bind_srvs();
		resolve_gBuffer();

		direct.context4->RSSetState(direct.rasterizer_state.Get());
		direct.context4->OMSetDepthStencilState(depth_stencil.read_only_state.Get(), 1);
		skybox.render();

		direct.context4->OMSetBlendState(direct.blend_state.Get(), 0, 0xffffffff);
		particle_system.render(camera, g_buffer.normalsCopy.srv, depth_stencil.srv);

		post_process.resolve(hdr_buffer, target_buffer);
	}
}
