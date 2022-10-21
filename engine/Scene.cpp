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

void Scene::init_depth_stencil_buffer(uint32_t width, uint32_t height)
{
	comptr<ID3D11Texture2D> buffer;

	// MSAA
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = hdr_buffer.msaa;
	depthBufferDesc.SampleDesc.Quality = 0;
	HRESULT result = Direct3D::instance().device5->CreateTexture2D(&depthBufferDesc, nullptr, &buffer);
	assert(SUCCEEDED(result) && "CreateTexture2D Depth Stencil");

	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc{};
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthViewDesc.Texture2D.MipSlice = 0;

	result = Direct3D::instance().device5->CreateDepthStencilView(buffer.Get(),
		&depthViewDesc, &depth_stencil.msaa_view);
	assert(SUCCEEDED(result) && "CreateDepthStencilView");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	result = Direct3D::instance().device5->CreateShaderResourceView(buffer.Get(), &srvDesc, &depth_stencil.msaa_srv);
	assert(SUCCEEDED(result) && "Depth CreateShaderResourceView");

	// Non-MSAA
	depthBufferDesc = {};
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	result = Direct3D::instance().device5->CreateTexture2D(&depthBufferDesc, nullptr, &buffer);
	assert(SUCCEEDED(result) && "CreateTexture2D Depth Stencil");

	depthViewDesc = {};
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Texture2D.MipSlice = 0;

	result = Direct3D::instance().device5->CreateDepthStencilView(buffer.Get(),
		&depthViewDesc, &depth_stencil.view);
	assert(SUCCEEDED(result) && "CreateDepthStencilView");

	srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1u;
	result = Direct3D::instance().device5->CreateShaderResourceView(buffer.Get(), &srvDesc, &depth_stencil.srv);
	assert(SUCCEEDED(result) && "Depth CreateShaderResourceView");
}

void Scene::init_depth_stencil_state()
{
	HRESULT result;

	D3D11_DEPTH_STENCIL_DESC depthDesc{};
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	result = Direct3D::instance().device5->CreateDepthStencilState(&depthDesc, &depth_stencil.state);
	assert(SUCCEEDED(result) && "CreateDepthStencilState");

	D3D11_DEPTH_STENCIL_DESC noDepthDesc{};
	noDepthDesc.DepthEnable = true;
	noDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	noDepthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	result = Direct3D::instance().device5->CreateDepthStencilState(&noDepthDesc, &depth_stencil.no_depth_state);
	assert(SUCCEEDED(result) && "CreateDepthStencilState");
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

	if(light_system.plights().size())
	{
		light_system.bind_point_dsv();
		mesh_system.opaque_instances.pointShadowShader->bind();
		mesh_system.opaque_instances.shadow_render(light_system.plights().size());
		mesh_system.appearing_instances.pointShadowShader->bind();
		mesh_system.appearing_instances.shadow_render(light_system.plights().size());
		grassfield.pointShadowShader->bind();
		grassfield.shadow_render(light_system.plights().size());
	}

	if(light_system.slights().size())
	{
		light_system.bind_spot_dsv();
		mesh_system.opaque_instances.spotShadowShader->bind();
		mesh_system.opaque_instances.shadow_render(light_system.slights().size());
	}
}

void Scene::bind_srvs()
{
	Direct3D& direct = Direct3D::instance();

	direct.context4->PSSetShaderResources(1, 1, skybox.irradiance_map.GetAddressOf());
	direct.context4->PSSetShaderResources(2, 1, skybox.reflection.map.GetAddressOf());
	direct.context4->PSSetShaderResources(3, 1, LightSystem::instance().depthBuffer.point_srv.GetAddressOf());
	direct.context4->PSSetShaderResources(4, 1, LightSystem::instance().depthBuffer.spot_srv.GetAddressOf());
}

void Scene::render(RenderBuffer& target_buffer, PostProcess& post_process, const Camera& camera)
{
	Direct3D& direct = Direct3D::instance();
	ParticleSystem& particle_system = ParticleSystem::instance();

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
	MeshSystem::instance().render();
	skybox.render();

	// Masked objects
	direct.context4->OMSetBlendState(direct.atc_blend_state.Get(), 0, 0xffffffff);
	grassfield.render();

	// Resolving msaa depth buffer before drawing appearing instances
	direct.resolve_depth(depth_stencil.msaa_srv, depth_stencil.view, hdr_buffer.msaa);

	// Appearing instances
	hdr_buffer.bind_rtv(depth_stencil.msaa_view);
	MeshSystem::instance().appearing_instances.render();

	// Render semi-transparent objects;
	direct.context4->OMSetDepthStencilState(depth_stencil.no_depth_state.Get(), 1);
	direct.context4->OMSetBlendState(direct.blend_state.Get(), 0, 0xffffffff);
	particle_system.render(camera, depth_stencil.srv);

	// Resolving hdr-buffer to output
	post_process.resolve(hdr_buffer, target_buffer);
	direct.context4->PSSetShaderResources(0, 1, &NULL_SRV);

	if (ImGuiManager::active())
		ImGuiManager::flush();
}
