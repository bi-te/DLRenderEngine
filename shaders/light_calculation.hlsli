#ifndef _LIGHT_CALCULATION_
#define  _LIGHT_CALCULATION_

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

uint select_cube_face(float3 unitDir)
{
	float maxVal = max(abs(unitDir.x), max(abs(unitDir.y), abs(unitDir.z)));
	uint index = abs(unitDir.x) == maxVal ? 0 : (abs(unitDir.y) == maxVal ? 2 : 4);
	return index + (asuint(unitDir[index / 2]) >> 31);
}

float point_shadow_calc(float3 world_position, float3 normal, uint index)
{
	PointLight pLight = g_lighting.pointLights[index];
	PointLightTransBuffer pTrans = g_lighting.pointTrans[index];


	float3 light_vec = normalize(pLight.position - world_position);
	uint face = select_cube_face(-light_vec);

	float3 compare_point = world_position + light_vec * offset;
	float4 compare = mul(pTrans.light_view_proj[face], float4(compare_point, 1.f));
	compare.xyz /= compare.w;
	float tex_size = 2.f * compare.w / g_lighting.buffer_side;

	float3 sample_point = world_position + normal * tex_size;

	return g_shadows.SampleCmp(g_comparison_sampler, float4(normalize(sample_point - pLight.position), index), compare.z);
}

float spot_shadow_calc(float3 world_position, float3 normal, uint index)
{
	Spotlight sLight = g_lighting.spotlights[index];
	SpotlightTransBuffer sTrans = g_lighting.spotTrans[index];

	float3 light_vec = normalize(sLight.position - world_position);

	float3 compare_point = world_position + light_vec * offset;
	float4 compare = mul(sTrans.light_view_proj, float4(compare_point, 1.f));
	compare.xyz /= compare.w;
	
	float tex_size = 2.f * compare.w * tan(sLight.outerCutOff)/ g_lighting.buffer_side;

	float4 sample_point = mul(sTrans.light_view_proj, float4(world_position + normal * tex_size, 1.f));
	sample_point /= sample_point.w;
	sample_point.x = sample_point.x * 0.5f + 0.5f;
	sample_point.y = 1 - sample_point.y * 0.5f - 0.5f;

	return g_spot_shadows.SampleCmp(g_comparison_sampler, float3(sample_point.xy, index), compare.z);
}

float3 calc_direct_light_pbr(float3 view_vec, float3 mesh_normal, float3 normal, DirectLight dirlight, Material mat)
{
	if (dot(-dirlight.direction, mesh_normal) <= 0.f) return 0.f;

	float3 h = normalize(view_vec - dirlight.direction);
	float3 f0 = lerp(INSULATOR_F0, mat.diffuse, mat.metallic);

	float cosNV = max(saturate(dot(normal, view_vec)), 0.001f);
	float cosNL = max(dot(normal, -dirlight.direction), 0.001f);
	float cosNH = max(dot(normal, h), 0.001f);
	float cosHL = max(dot(h, -dirlight.direction), 0.001f);

	float rough2 = mat.roughness * mat.roughness;

	float g = ggx_smith(rough2, cosNV, cosNL);
	float d = min(ggx_distribution(rough2, cosNH) * dirlight.solid_angle * 0.25f / cosNV, 1.f);
	float3 f = fresnel(f0, cosHL);

	float3 diffK = float3(1.f, 1.f, 1.f) - fresnel(f0, cosNL);

	float3 spec = f * g * d;
	float3 diff = (1 - mat.metallic) * diffK * mat.diffuse * dirlight.solid_angle * cosNL / PI;

	float3 color = (diff + spec) * dirlight.radiance;

	return color;
}

float3 pbr_point(float3 light, float3 closest_light, float light_dist, float3 normal, float3 mesh_normal, float3 view,
	float3 radiance, float radius, float solidAngle, Material mat)
{
	float3 h = normalize(closest_light + view);
	float3 f0 = lerp(INSULATOR_F0, mat.diffuse, mat.metallic);

	float cosNL = dot(normal, light);
	float cosMNL = dot(mesh_normal, light);

	float lightMicroHeight = cosNL * light_dist;
	float lightMacroHeight = cosMNL * light_dist;

	float fadingMicro = saturate((lightMicroHeight + radius) / (2.f * radius));
	float fadingMacro = saturate((lightMacroHeight + radius) / (2.f * radius));
	float sphereSin = radius / light_dist;

	cosNL = max(cosNL, fadingMicro * sphereSin);
	float cosNV = max(saturate(dot(normal, view)), 0.001f);
	float cosNCL = max(dot(normal, closest_light), 0.001f);
	float cosNH = max(dot(normal, h), 0.001f);
	float cosHCL = max(dot(h, closest_light), 0.001f);

	float rough2 = mat.roughness * mat.roughness;

	float g = ggx_smith(rough2, cosNV, cosNCL);
	float d = min(ggx_distribution(rough2, cosNH) * solidAngle * 0.25f / cosNV, 1.f);
	float3 f = fresnel(f0, cosHCL);

	float3 diffK = float3(1.f, 1.f, 1.f) - fresnel(f0, cosNL);

	float3 spec = f * g * d;
	float3 diff =  (1 - mat.metallic) * diffK * mat.diffuse * solidAngle * cosNL / PI;

	float3 color = fadingMacro * fadingMicro * (diff + spec) * radiance;
	return color;
}

float3 calc_point_light_pbr(float3 position, float3 view_vec, float3 mesh_normal, float3 normal, Material material)
{
	float3 color = 0.f;
	for (uint pLight_ind = 0; pLight_ind < g_lighting.pointLightNum; ++pLight_ind)
	{
		PointLight pointLight = g_lighting.pointLights[pLight_ind];

		float depth = point_shadow_calc(position, mesh_normal, pLight_ind);

		float3 light_vec = pointLight.position - position;
		float light_dist = max(length(light_vec), pointLight.radius);
		light_vec = normalize(light_vec);

		float sphereCos = sqrt(light_dist * light_dist - pointLight.radius * pointLight.radius) / light_dist;
		float solidAngle = (1 - sphereCos) * 2.f * PI;

		float3 closest_vec = closest_sphere_direction(light_vec * light_dist, light_vec,
			reflect(-view_vec, normal), light_dist, pointLight.radius, sphereCos);
		closest_vec = clamp_to_horizon(normal, closest_vec, 0.001f);

		color += depth * pbr_point(light_vec, closest_vec, light_dist, normal, mesh_normal, view_vec,
			pointLight.radiance, pointLight.radius, solidAngle, material);
	}
	return color;

}

float3 calc_spotlight_pbr(float3 position, float3 view_vec, float3 mesh_normal, float3 normal, Material material)
{
	float3 color = 0.f;
	for (uint sLight_ind = 0; sLight_ind < g_lighting.spotlightNum; ++sLight_ind)
	{
		Spotlight spotlight = g_lighting.spotlights[sLight_ind];

		float3 light_vec = spotlight.position - position;
		float light_dist = max(length(light_vec), spotlight.radius);
		light_vec = normalize(light_vec);

		if (dot(light_vec, mesh_normal) <= 0.f) return 0.f;

		float cosDSL = dot(spotlight.direction, -light_vec);
		float intensity = smoothstep(cos(spotlight.outerCutOff), cos(spotlight.cutOff), cosDSL);
		if (intensity == 0.f) continue;

		float sphereCos = sqrt(light_dist * light_dist - spotlight.radius * spotlight.radius) / light_dist;
		float solidAngle = (1.f - sphereCos) * 2.f * PI * intensity;

		float3 closest_vec = closest_sphere_direction(light_vec * light_dist, light_vec,
			reflect(-view_vec, normal), light_dist, spotlight.radius, sphereCos);
		clamp_to_horizon(normal, closest_vec, 0.001f);

		float depth = spot_shadow_calc(position, mesh_normal, sLight_ind);
		color += depth * pbr_point(light_vec, closest_vec, light_dist, normal, mesh_normal, view_vec,
			spotlight.radiance, spotlight.radius, solidAngle, material);
	}
	return color;
}

float3 calc_environment_light(float3 view_vec, float3 normal, Material mat)
{
	float3 diffuse = mat.diffuse * (1.f - mat.metallic) * g_irradiance.Sample(g_linear_clamp_sampler, normal.xyz).rgb;

	float cosNV = dot(normal, view_vec);
	if (cosNV < 0) return diffuse;

	cosNV = max(cosNV, 0.0001f);

	float3 f0 = lerp(INSULATOR_F0, mat.diffuse, mat.metallic);
	float3 reflection = reflect(-view_vec, normal);

	float2 lut_coor = float2(mat.roughness, 1.f - cosNV);
	float2 reflectanceLUT = g_reflectance.Sample(g_linear_clamp_sampler, lut_coor).rg;
	float3 reflectance = f0 * reflectanceLUT.x + reflectanceLUT.y;
	float3 specular = reflectance * g_reflection.SampleLevel(g_linear_clamp_sampler, reflection, mat.roughness * g_max_reflection_mip).rgb;

	return diffuse + specular;
}

#endif