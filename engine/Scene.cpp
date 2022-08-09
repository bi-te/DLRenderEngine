#include "Scene.h"

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/ImGuiManager.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "moving/SphereMover.h"
#include "moving/PointLightMover.h"
#include "moving/SpotlightMover.h"
#include "moving/TransformMover.h"
#include "render/MeshSystem.h"
#include "render/Direct11/Direct3D.h"

bool Scene::select_object(const Ray& ray, float t_min, float t_max, IntersectionQuery& record)
{
	bool intersection = false;
	objectRef ref;
	record.intersection = Intersection::infinite();

	switch (ref.type)
	{
	case SPHERE: record.mover = std::make_unique<SphereMover>(static_cast<SphereObject*>(ref.ptr)->sphere); break;
	case MESH: record.mover = std::make_unique<TransformMover>(static_cast<MeshInstance*>(ref.ptr)->transform); break;
	case POINTLIGHT: record.mover = std::make_unique<PointLightMover>(*static_cast<PointLightObject*>(ref.ptr)); break;
	case SPOTLIGHT: record.mover = std::make_unique<SpotlightMover>(*static_cast<SpotlightObject*>(ref.ptr)); break;
	case NONE: record.mover = nullptr;
	}

	return intersection;
}

void Scene::init_depth_and_stencil_buffer(uint32_t width, uint32_t height)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc{};
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	HRESULT result = Direct3D::instance().device5->CreateTexture2D(&depthBufferDesc, nullptr,
		&depth_stencil.buffer);
	assert(SUCCEEDED(result) && "CreateTexture2D Depth Stencil");

	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc{};
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
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

void Scene::draw(Window& window)
{
	window.clear_buffer();
	window.bind_target(depth_stencil.view);
	Direct3D::instance().context4->ClearDepthStencilView(depth_stencil.view.Get(), D3D11_CLEAR_DEPTH, 0.f, 0);
	Direct3D::instance().context4->RSSetState(Direct3D::instance().rasterizer_state.Get());
	Direct3D::instance().context4->OMSetDepthStencilState(depth_stencil.state.Get(), 1);

	MeshSystem::instance().render();

	skybox.draw();

	if(ImGuiManager::active())
		ImGuiManager::flush();

	window.swap_buffer();
}
