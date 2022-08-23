#include "LightSystem.h"

#include "MeshSystem.h"
#include "Direct11/Direct3D.h"
#include "moving/TransformSystem.h"
#include <render/ModelManager.h>

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
}

void LightSystem::add_point_light(const PointLight& pointLight, const std::string& model)
{
	pointLights.push_back(pointLight);
	
	vec3f emissive_light = irradianceAtDistanceToRadiance(pointLight.irradiance, pointLight.light_range, pointLight.radius);

	MeshSystem::instance().emissive_instances.add_model_instance(
		ModelManager::instance().get_ptr(model), 
		{emissive_light, pointLight.position}
	);
}

void LightSystem::add_spotlight(const Spotlight& spotlight)
{
	spotlights.push_back(spotlight);
}

void LightSystem::add_spotlight(const Spotlight& spotlight, const std::string& model)
{
	spotlights.push_back(spotlight);
	
	vec3f emissive_light = irradianceAtDistanceToRadiance(spotlight.irradiance, spotlight.light_range, spotlight.radius);

	MeshSystem::instance().emissive_instances.add_model_instance(
		ModelManager::instance().get_ptr(model),
		{ emissive_light, spotlight.position }
	);
}

void LightSystem::bind_lights()
{
	TransformSystem& t_system = TransformSystem::instance();

	lightBuffer.allocate(sizeof(LightBuffer));

	LightBuffer* lBuffer = static_cast<LightBuffer*>(lightBuffer.map().pData);
	lBuffer->ambient = ambient;
	lBuffer->pointLightNum = pointLights.size();
	lBuffer->spotlightNum = spotlights.size();
	lBuffer->dirLight = dirLight;

	for (int pLight = 0; pLight < pointLights.size(); ++pLight)
	{
		PointLight& p_data = pointLights[pLight];
		PointLightBuffer& p_buffer = lBuffer->pointLights[pLight];

		p_buffer.radius = p_data.radius;
		p_buffer.position = t_system.transforms[p_data.position].position();
		p_buffer.radiance = irradianceAtDistanceToRadiance(p_data.irradiance, p_data.light_range, p_data.radius);
	}

	for (int sLight = 0; sLight < spotlights.size(); ++sLight)
	{
		Spotlight& s_data = spotlights[sLight];
		SpotlightBuffer& s_buffer = lBuffer->spotlights[sLight];

		s_buffer.radius = s_data.radius;
		s_buffer.cutOff = s_data.cutOff;
		s_buffer.position = t_system.transforms[s_data.position].position();
		s_buffer.direction = s_data.direction;
		s_buffer.radiance = irradianceAtDistanceToRadiance(s_data.irradiance, s_data.light_range, s_data.radius);	
		s_buffer.outerCutOff = s_data.outerCutOff;
	}

	lightBuffer.unmap();

	Direct3D::instance().context4->PSSetConstantBuffers(1, 1, lightBuffer.address());
}
