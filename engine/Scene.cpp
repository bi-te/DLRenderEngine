#include "Scene.h"

#include <iostream>

#include "PostProcess.h"
#include "imgui/imgui.h"
#include "imgui/ImGuiManager.h"
#include "render/LightSystem.h"
#include "render/MeshSystem.h"
#include "render/ShaderManager.h"
#include "render/Direct11/Direct3D.h"

void Scene::init_depth_stencil_buffer(uint32_t width, uint32_t height)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = hdr_buffer.msaa;
	depthBufferDesc.SampleDesc.Quality = 0;
	HRESULT result = Direct3D::instance().device5->CreateTexture2D(&depthBufferDesc, nullptr,
		&depth_stencil.buffer);
	assert(SUCCEEDED(result) && "CreateTexture2D Depth Stencil");

	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc{};
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthViewDesc.Texture2D.MipSlice = 0;

	result = Direct3D::instance().device5->CreateDepthStencilView(depth_stencil.buffer.Get(),
		&depthViewDesc, &depth_stencil.view);
	assert(SUCCEEDED(result) && "CreateDepthStencilView");
}

void Scene::init_depth_stencil_state()
{
	D3D11_DEPTH_STENCIL_DESC depthDesc{};
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	HRESULT result = Direct3D::instance().device5->CreateDepthStencilState(&depthDesc, &depth_stencil.state);
	assert(SUCCEEDED(result) && "CreateDepthStencilState");
}

void Scene::init_hdr_and_depth_buffer(uint32_t width, uint32_t height, uint32_t msaa)
{
	hdr_buffer.create(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, msaa);

	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = msaa;
	depthBufferDesc.SampleDesc.Quality = 0;
	HRESULT result = Direct3D::instance().device5->CreateTexture2D(&depthBufferDesc, nullptr,
		&depth_stencil.buffer);
	assert(SUCCEEDED(result) && "CreateTexture2D Depth Stencil");

	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc{};
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	depthViewDesc.Texture2D.MipSlice = 0;

	result = Direct3D::instance().device5->CreateDepthStencilView(depth_stencil.buffer.Get(),
		&depthViewDesc, &depth_stencil.view);
	assert(SUCCEEDED(result) && "CreateDepthStencilView");
}
void Scene::shadow_pass()
{
	Direct3D& direct = Direct3D::instance();
	LightSystem& light_system = LightSystem::instance();
	
	shadowShader->bind();

	direct.context4->PSSetShaderResources(3, 1, &NULL_SRV);
	light_system.bind_depth_state();

	uint32_t lightNum = light_system.plights().size();
	for (uint32_t light = 0; light < lightNum; light++)
	{
		light_system.bind_shadow_light(light);
		MeshSystem::instance().opaque_instances.mesh_render();
	}
}

void Scene::render(RenderBuffer& target_buffer, PostProcess& post_process, const Camera& camera)
{
	Direct3D& direct = Direct3D::instance();

	direct.bind_globals(camera, skybox.reflection.mip_levels - 1u);

	shadow_pass();

	hdr_buffer.clear();
	hdr_buffer.bind_rtv(depth_stencil.view);


	direct.context4->ClearDepthStencilView(depth_stencil.view.Get(), D3D11_CLEAR_DEPTH, 0.f, 0);
	direct.context4->RSSetState(direct.rasterizer_state.Get());
	direct.context4->OMSetDepthStencilState(depth_stencil.state.Get(), 1);
		
	direct.context4->PSSetShaderResources(1, 1, skybox.irradiance_map.GetAddressOf());
	direct.context4->PSSetShaderResources(2, 1, skybox.reflection.map.GetAddressOf());
	direct.context4->PSSetShaderResources(3, 1, LightSystem::instance().depthBuffer.srv.GetAddressOf());

	MeshSystem::instance().render();
	skybox.render();
	
	post_process.resolve(hdr_buffer, target_buffer);
	direct.context4->PSSetShaderResources(0, 1, &NULL_SRV);

	if (ImGuiManager::active())
		ImGuiManager::flush();
}
