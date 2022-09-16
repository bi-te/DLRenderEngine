#include "LightSystem.h"

#include "MeshSystem.h"
#include "Direct11/Direct3D.h"
#include "moving/TransformSystem.h"
#include "render/ModelManager.h"

LightSystem* LightSystem::s_system;

vec3f irradianceAtDistanceToRadiance(const vec3f& irradiance, float distance, float radius)
{
	float angleSin = std::min(1.f, radius / distance);
	float angleCos = sqrtf(1.f - angleSin * angleSin);
	float occupation = 1.f - angleCos;
	return irradiance / occupation;
}

void LightSystem::set_ambient(const vec3f& ambient_color)
{
	ambient = ambient_color;
}

void LightSystem::set_direct_light(const DirectLight& dirLight)
{
	this->dirLight = dirLight;
}

void LightSystem::add_point_light(const PointLight & pointLight)
{
	pointLights.push_back(pointLight);
	pointLights.back().radiance = irradianceAtDistanceToRadiance(pointLight.irradiance, pointLight.light_range, pointLight.radius);
}

void LightSystem::add_point_light(const PointLight& pointLight, const std::string& model)
{
	vec3f emissive_light = irradianceAtDistanceToRadiance(pointLight.irradiance, pointLight.light_range, pointLight.radius);

	pointLights.push_back(pointLight);
	pointLights.back().radiance = emissive_light;

	MeshSystem::instance().emissive_instances.add_model_instance(
		ModelManager::instance().get_ptr(model), 
		{emissive_light, pointLight.position}
	);
}

void LightSystem::add_spotlight(const Spotlight& spotlight)
{
	spotlights.push_back(spotlight);
	spotlights.back().radiance = irradianceAtDistanceToRadiance(spotlight.irradiance, spotlight.light_range, spotlight.radius);
}

void LightSystem::add_spotlight(const Spotlight& spotlight, const std::string& model)
{
	vec3f emissive_light = irradianceAtDistanceToRadiance(spotlight.irradiance, spotlight.light_range, spotlight.radius);

	spotlights.push_back(spotlight);
	spotlights.back().radiance = emissive_light;

	MeshSystem::instance().emissive_instances.add_model_instance(
		ModelManager::instance().get_ptr(model),
		{ emissive_light, spotlight.position }
	);
}

void LightSystem::bind_point_shadow_light(uint32_t index)
{
	Direct3D& direct = Direct3D::instance();

	ShadowBuffer* ltb = static_cast<ShadowBuffer*>(lightTransformBuffer.map().pData);
	ltb->index = index;
	lightTransformBuffer.unmap();
	
	direct.context4->GSSetConstantBuffers(2, 1, lightTransformBuffer.address());

	direct.context4->ClearDepthStencilView(depthBuffer.views[index].Get(), D3D11_CLEAR_DEPTH, 0.f, 0);
	direct.context4->OMSetRenderTargets(1, &NULL_RTV, depthBuffer.views[index].Get());
}

void LightSystem::bind_spot_shadow_light(uint32_t index)
{
	Direct3D& direct = Direct3D::instance();

	ShadowBuffer* ltb = static_cast<ShadowBuffer*>(lightTransformBuffer.map().pData);
	ltb->index = index;
	lightTransformBuffer.unmap();

	direct.context4->VSSetConstantBuffers(2, 1, lightTransformBuffer.address());

	direct.context4->ClearDepthStencilView(depthBuffer.spot_views[index].Get(), D3D11_CLEAR_DEPTH, 0.f, 0);
	direct.context4->OMSetRenderTargets(1, &NULL_RTV, depthBuffer.spot_views[index].Get());
}

void LightSystem::bind_lights(LightBuffer* lBuffer)
{
	TransformSystem& t_system = TransformSystem::instance();

	lBuffer->ambient = ambient;
	lBuffer->pointLightNum = std::min(static_cast<uint32_t>(pointLights.size()), MAX_LIGHTS_NUMBER);
	lBuffer->spotlightNum = std::min(static_cast<uint32_t>(spotlights.size()), MAX_LIGHTS_NUMBER);
	lBuffer->dirLight = dirLight;
	lBuffer->shadow_near = shadow_near;
	lBuffer->shadow_far = shadow_far;
	lBuffer->buffer_side_size = depthBuffer.viewport.Width;

	for (uint32_t pLight = 0; pLight < lBuffer->pointLightNum; ++pLight)
	{
		PointLight& p_data = pointLights[pLight];
		PointLightBuffer& p_buffer = lBuffer->pointLights[pLight];
		PointLightTransBuffer& trans = lBuffer->pointTrans[pLight];

		p_buffer.radius = p_data.radius;
		p_buffer.position = t_system.transforms[p_data.position].position();
		p_buffer.radiance = p_data.radiance;

		trans.light_proj = perspective_proj(rad(90.f), 1.f, shadow_near, shadow_far);
		trans.light_view[0] = lookAt(p_buffer.position, WORLD_Y, { 1.f,  0.f,  0.f });
		trans.light_view[1] = lookAt(p_buffer.position, WORLD_Y, { -1.f,  0.f,  0.f });
		trans.light_view[2] = lookAt(p_buffer.position, -WORLD_Z, { 0.f,  1.f,  0.f });
		trans.light_view[3] = lookAt(p_buffer.position, WORLD_Z, { 0.f, -1.f,  0.f });
		trans.light_view[4] = lookAt(p_buffer.position, WORLD_Y, { 0.f,  0.f,  1.f });
		trans.light_view[5] = lookAt(p_buffer.position, WORLD_Y, { 0.f,  0.f, -1.f });
	}

	for (uint32_t sLight = 0; sLight < lBuffer->spotlightNum; ++sLight)
	{
		Spotlight& s_data = spotlights[sLight];
		SpotlightBuffer& s_buffer = lBuffer->spotlights[sLight];
		SpotlightTransBuffer& trans = lBuffer->spotTrans[sLight];

		s_buffer.radius = s_data.radius;
		s_buffer.cutOff = s_data.cutOff;
		s_buffer.position = t_system.transforms[s_data.position].position();
		s_buffer.direction = s_data.direction;
		s_buffer.radiance = s_data.radiance;
		s_buffer.outerCutOff = s_data.outerCutOff;
		trans.light_view = lookAt(t_system.transforms[s_data.position], s_data.direction);
		trans.light_proj = perspective_proj(2.f * s_data.outerCutOff, 1.f, shadow_near, shadow_far);
	}
}

void LightSystem::bind_depth_state()
{
	Direct3D& direct = Direct3D::instance();
	direct.context4->RSSetViewports(1, &depthBuffer.viewport);
	direct.context4->OMSetDepthStencilState(depthBuffer.state.Get(), 1);
}

void LightSystem::init_depth_buffers(uint32_t side_size)
{
	Direct3D& direct = Direct3D::instance();
	HRESULT res;

	depthBuffer.viewport.Width = side_size;
	depthBuffer.viewport.Height = side_size;

	uint32_t pLightsNum = pointLights.size();
	uint32_t sLightsNum = spotlights.size();

	D3D11_TEXTURE2D_DESC texture_desc{};
	texture_desc.Format = DXGI_FORMAT_R32_TYPELESS;
	texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture_desc.Height = side_size;
	texture_desc.Width = side_size;
	texture_desc.MipLevels = 1u;
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.SampleDesc.Count = 1u;

	comptr<ID3D11Texture2D> depthTexture;

	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc{};
	srv_desc.Format = DXGI_FORMAT_R32_FLOAT;

	comptr<ID3D11DepthStencilView> depthView;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc{};
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthViewDesc.Texture2DArray.MipSlice = 0u;

	if(pLightsNum)
	{
		texture_desc.ArraySize = pLightsNum * 6u;
		texture_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		
		res = direct.device5->CreateTexture2D(&texture_desc, nullptr, &depthTexture);
		assert(SUCCEEDED(res) && "CreateTexture2D LightDepthBuffer");

		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		srv_desc.TextureCubeArray.NumCubes = pLightsNum;
		srv_desc.TextureCubeArray.MipLevels = 1u;
		srv_desc.TextureCubeArray.MostDetailedMip = 0u;
		res = direct.device5->CreateShaderResourceView(depthTexture.Get(), &srv_desc, &depthBuffer.srv);
		assert(SUCCEEDED(res) && "CreateShaderResourceView LightDepthBuffer");
		
		depthViewDesc.Texture2DArray.ArraySize = 6u;
		for (uint32_t light = 0; light < pLightsNum; ++light)
		{
			depthViewDesc.Texture2DArray.FirstArraySlice = light * 6u;
			res = direct.device5->CreateDepthStencilView(depthTexture.Get(), &depthViewDesc, &depthView);
			assert(SUCCEEDED(res) && "CreateDepthStencilView LightDepthBuffer");
			depthBuffer.views.push_back(std::move(depthView));
		}
	}

	if(sLightsNum)
	{
		texture_desc.ArraySize = sLightsNum;
		texture_desc.MiscFlags = 0;

		res = direct.device5->CreateTexture2D(&texture_desc, nullptr, &depthTexture);
		assert(SUCCEEDED(res) && "CreateTexture2D LightDepthBuffer");

		srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srv_desc.Texture2DArray.ArraySize = sLightsNum;
		srv_desc.Texture2DArray.MipLevels = 1u;
		srv_desc.Texture2DArray.MostDetailedMip = 0u;
		res = direct.device5->CreateShaderResourceView(depthTexture.Get(), &srv_desc, &depthBuffer.spot_srv);
		assert(SUCCEEDED(res) && "CreateShaderResourceView LightDepthBuffer");

		depthViewDesc.Texture2DArray.ArraySize = 1u;
		for (uint32_t light = 0; light < sLightsNum; ++light)
		{
			depthViewDesc.Texture2DArray.FirstArraySlice = light;
			res = direct.device5->CreateDepthStencilView(depthTexture.Get(), &depthViewDesc, &depthView);
			assert(SUCCEEDED(res) && "CreateDepthStencilView LightDepthBuffer");
			depthBuffer.spot_views.push_back(std::move(depthView));
		}
	}
	
	D3D11_DEPTH_STENCIL_DESC depthDesc{};
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	res = direct.device5->CreateDepthStencilState(&depthDesc, &depthBuffer.state);
	assert(SUCCEEDED(res) && "CreateDepthStencilState LightDepthBuffer");
}
