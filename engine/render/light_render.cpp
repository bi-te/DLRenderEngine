#include "light_render.h"

#include <algorithm>
#include <iostream>

#include "../math/Ray.h"

//Blinn-Phong Model
void calc_direct_light(vec3& color, const DirectLight& dirlight, const Intersection& record, const vec3& camera_pos,
                       const Material& m)
{
	float visibility = std::max(record.norm.dot(-dirlight.direction), 0.f);
	if (visibility == 0.f) return;

	vec3 cameravec = (camera_pos - record.point).normalized();
	vec3 h = (cameravec - dirlight.direction).normalized();

	color += dirlight.light.cwiseProduct(visibility * m.albedo) +
		dirlight.light * powf(h.dot(record.norm), m.glossiness) * m.specular;
}

void calc_point_light(vec3& color, const PointLight& plight, const Intersection& record, const vec3& camera_pos,
	const Material& m)
{
	vec3 light_dir = plight.position - record.point;
	float light_dist = light_dir.norm();
	light_dir.normalize();

	float visibility = std::max(record.norm.dot(light_dir), 0.f);
	if (visibility == 0.f) return;

	vec3 cameravec = (camera_pos - record.point).normalized();
	vec3 h = (cameravec + light_dir).normalized();

	color += plight.light.cwiseProduct(visibility * m.albedo) / powf(light_dist / plight.light_range, 2.f)
		+ plight.light * powf(h.dot(record.norm), m.glossiness) * m.specular;
}

void calc_spotlight(vec3& color, const Spotlight& spotlight, const Intersection& record, const vec3& camera_pos,
	const Material& m)
{
	vec3 light_dir = spotlight.position - record.point;
	float light_dist = light_dir.norm();
	light_dir.normalize();

	float cos = spotlight.direction.dot(-light_dir);
	float intensity = smoothstep(spotlight.outerCutOff, spotlight.cutOff, cos);
	if (intensity == 0.f) return;

	vec3 cameravec = (camera_pos - record.point).normalized();
	vec3 h = (cameravec + light_dir).normalized();

	color += intensity * (spotlight.light.cwiseProduct(std::max(record.norm.dot(light_dir), 0.f) * m.albedo) / powf(light_dist / spotlight.light_range, 2.f)
		+ spotlight.light * powf(std::max(h.dot(record.norm), 0.f), m.glossiness) * m.specular);
}


//Cook-Tarrance
float ggx_distribution(float rough2, float cosH)
{
	float denum = cosH * cosH * (rough2 - 1) + 1;
	denum = PI * denum * denum;
	return rough2 / denum;
}

float ggx_smith(float rough2, float cosV, float cosL)
{
	cosV *= cosV;
	cosL *= cosL;
	return 2 / (sqrtf(1 + rough2 * (1 - cosV) / cosV) + sqrtf(1 + rough2 * (1 - cosL) / cosL));
}

vec3 fresnel(const vec3& f0, float cosL)
{
	return f0 + (vec3(1.f, 1.f, 1.f) - f0) * powf(1.f - cosL, 5.f);
}

void cook_torrance_aprox(vec3& color, const vec3& l, const vec3& cl, const vec3& n, const vec3& v, 
	const vec3& radiance, float dw, const Material& m)
{
	vec3 h = (cl + v).normalized();

	float cosV = n.dot(v);
	if (cosV <= 0.f) { return; }
	float cosL = n.dot(l);
	if (cosL <= 0.f) { return; }
	float cosC = n.dot(cl);
	float cosH = n.dot(h);
	float cosHL = h.dot(cl);

	float rough2 = m.roughness * m.roughness;
	float g = ggx_smith(rough2, cosV, cosC);
	float d = std::min(ggx_distribution(rough2, cosH) * dw * 0.25f / (cosV * cosL), 1.f);
	vec3 f = fresnel(m.f0, cosHL);
	vec3 diffK = vec3(1.f, 1.f, 1.f) - fresnel(m.f0, cosL);

	vec3 spec = f * g * d;
	vec3 diff = (1 - m.metalness) * diffK.cwiseProduct(m.albedo) * dw / PI;

	color += (diff + spec ).cwiseProduct(radiance) * cosL;
}

void cook_torrance(vec3& color, const vec3& l, const vec3& n, const vec3& v,
	const vec3& radiance, float dw, const Material& m)
{
	vec3 h = (l + v).normalized();

	float cosV = n.dot(v);
	if (cosV <= 0.f) { return; }
	float cosL = n.dot(l);
	if (cosL <= 0.f) { return; }
	float cosH = n.dot(h);
	float cosHL = h.dot(l);

	float rough2 = m.roughness * m.roughness;
	float g = ggx_smith(rough2, cosV, cosL);
	float d = ggx_distribution(rough2, cosH);
	

	vec3 f = fresnel(m.f0, cosHL);
	vec3 diffK = vec3(1.f, 1.f, 1.f) - f;

	vec3 spec = f * g * std::min(d *dw * 0.25f / (cosV * cosL), 1.f);
	vec3 diff = (1 - m.metalness) * diffK.cwiseProduct(m.albedo) * dw  / PI;

	color += (diff + spec).cwiseProduct(radiance) * cosL;
}

// PBR
void calc_direct_light_pbr(vec3& color, const DirectLight& dirlight, const vec3& norm,
	const vec3& light_vec, const vec3& view, const Material& m)
{
	cook_torrance_aprox(color, light_vec, light_vec, norm, view, dirlight.light, dirlight.solid_angle / ( 2 * PI), m);
}

void calc_point_light_pbr(vec3& color, vec3 light_vec, const vec3& light, float light_dist, float radius, 
	const vec3& norm, const vec3& view, const Material& m)
{	
	float cosPhi = sqrtf(light_dist * light_dist - radius * radius) / light_dist;
	float attenuation = 1 - cosPhi;

	vec3 closest_vec = closest_sphere_direction(light_vec * light_dist, light_vec,
		reflect(-view, norm), light_dist, radius, cosPhi);
	clamp_to_horizon(norm, light_vec, 0.01f);
	cook_torrance_aprox(color, light_vec, closest_vec, norm, view, light, attenuation, m);
}

void calc_spotlight_pbr(vec3& color, const Spotlight& spotlight, float radius, 
	const Intersection& record, const vec3& view, const Material& m)
{
	vec3 light_vec = spotlight.position - record.point;
	float dist = light_vec.norm();
	light_vec.normalize();

	float cosDSL = spotlight.direction.dot(-light_vec);
	float intensity = smoothstep(spotlight.outerCutOff, spotlight.cutOff, cosDSL);
	if (intensity == 0.f) return;

	float cosPhi = sqrtf(dist * dist - radius * radius) / dist;
	float attenuation = (1.f - cosPhi) * intensity;

	vec3 closest_vec = closest_sphere_direction(light_vec * dist, light_vec,
		reflect(-view, record.norm), dist, radius, cosPhi);
	clamp_to_horizon(record.norm, light_vec, 0.01f);

	cook_torrance_aprox(color, light_vec, closest_vec, record.norm, view, spotlight.light, attenuation, m);
}

//Color processing
void adjust_exposure(vec3& color, float ev100)
{
	float lmax = 1.2f * powf(2.f, ev100);
	color *= (1.f / lmax);
}

void aces_tonemap(vec3& color)
{
	mat3 m1 {
		{0.59719f, 0.07600f, 0.02840f},
			{0.34548f, 0.90834f, 0.13383f},
			{0.04823f, 0.01566f, 0.83777f}
	};

	mat3 m2 {
		{1.60475f, -0.10208f, -0.00327f},
			{-0.53108f, 1.10813f, -0.07276f},
			{-0.07367f, -0.00605f, 1.07602f}
	};
	arr3 v = color * m1;
	arr3 a = v * (v + 0.0245786f) - 0.000090537f;
	arr3 b = v * (0.983729f * v + 0.4329510f )+ 0.238081f;
	vec3 temp = vec3{ a / b} * m2;

	color.x() = std::clamp(temp.x(), 0.0f, 1.f);
	color.y() = std::clamp(temp.y(), 0.0f, 1.f);
	color.z() = std::clamp(temp.z(), 0.0f, 1.f);
}

void gamma_correction(vec3& color)
{
	color.x() = powf(color.x(), 1.f / 2.2f);
	color.y() = powf(color.y(), 1.f / 2.2f);
	color.z() = powf(color.z(), 1.f / 2.2f);
}

//hemisphere integration

