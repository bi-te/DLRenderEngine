#ifndef _LIGHT_CALCULATION_
#define  _LIGHT_CALCULATION_

#include "globals.hlsli"
#include "pbr_render.hlsli"

float3 closest_sphere_direction(float3 sphere_rel_pos, float3 sphere_dir, float3 reflection,
	float sphere_dist, float radius, float cos_sphere)
{
	float cosRoS = dot(reflection, sphere_dir);

	if (cosRoS >= cos_sphere) return reflection;
	if (cosRoS < 0.f) return sphere_dir;

	float3 closes_point_dir = normalize(reflection * sphere_dist * cosRoS - sphere_rel_pos);
	return normalize(sphere_rel_pos + closes_point_dir * radius);
}

float3 clamp_to_horizon(float3 norm, float3 dir, float min_cos)
{
	float cosNoD = dot(norm, dir);
	if (cosNoD < 0.f)
		return normalize(dir + norm * (min_cos - cosNoD));
	return dir;
}

float3 calc_direct_light_pbr(float3 view_vec, float3 mesh_normal, float3 normal, DirectLight dirlight, Material mat)
{
	if (dot(-dirLight.direction, mesh_normal) < 0.f) return 0.f;
	return cook_torrance_aprox(-dirlight.direction, -dirlight.direction, normal, view_vec, dirlight.radiance, dirlight.solid_angle / (2 * PI), mat);
}

float3 calc_point_light_pbr(float3 position, float3 view_vec, float3 mesh_normal, float3 normal, PointLight pointLight, Material material)
{
	float3 light_vec = pointLight.position - position;
	float light_dist = max(length(light_vec), pointLight.radius);
	light_vec = normalize(light_vec);

	if (dot(light_vec, mesh_normal) < 0.f) return 0.f;

	float cosPhi = sqrt(light_dist * light_dist - pointLight.radius * pointLight.radius) / light_dist;
	float attenuation = 1 - cosPhi;

	float3 closest_vec = closest_sphere_direction(light_vec * light_dist, light_vec,
		reflect(-view_vec, normal), light_dist, pointLight.radius, cosPhi);

	closest_vec = clamp_to_horizon(normal, closest_vec, 0.01f);

	return cook_torrance_aprox(light_vec, closest_vec, normal, view_vec, pointLight.radiance, attenuation, material);
}

float3 calc_spotlight_pbr(float3 position, float3 view_vec, float3 mesh_normal, float3 normal, Spotlight spotlight, Material material)
{
	float3 light_vec = spotlight.position - position;
	float dist = max(length(light_vec), spotlight.radius);
	light_vec = normalize(light_vec);

	if (dot(light_vec, mesh_normal) < 0.f) return 0.f;

	float cosDSL = dot(spotlight.direction, -light_vec);
	float intensity = smoothstep(spotlight.outerCutOff, spotlight.cutOff, cosDSL);
	if (intensity == 0.f) return 0.f;

	float cosPhi = sqrt(dist * dist - spotlight.radius * spotlight.radius) / dist;
	float attenuation = (1.f - cosPhi) * intensity;

	float3 closest_vec = closest_sphere_direction(light_vec * dist, light_vec,
		reflect(-view_vec, normal), dist, spotlight.radius, cosPhi);
	clamp_to_horizon(normal, closest_vec, 0.01f);

	return cook_torrance_aprox(light_vec, closest_vec, normal, view_vec, spotlight.radiance, attenuation, material);
}

#endif