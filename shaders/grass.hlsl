#include "globals.hlsli"
#include "grass_helpers.hlsli"
#include "light_calculation.hlsli"

struct vs_in
{
	float3 position: Inst_Position;
	float2 rel_position: Inst_RelPosition;
};

cbuffer GrassBuffer: register(b1)
{
	uint g_planes;
	uint g_sectors;
	float2 g_scale;
}

grass_out main(uint index: SV_VertexID, vs_in input) 
{
	grass_properties gp;
	gp.position = input.position;
	gp.planes = g_planes;
	gp.sectors = g_sectors;
	gp.scale = g_scale;
	gp.rel_pos = input.rel_position;
	return grass_point(index, gp);
}

Texture2D g_diffuse : register(t5);
Texture2D g_normals: register(t6);
Texture2D g_roughness: register(t7);
Texture2D g_opacity: register(t8);
Texture2D g_translucency: register(t9);
Texture2D g_ao: register(t10);

float4 ps_main(grass_out input, bool isFrontFace: SV_IsFrontFace): SV_Target
{
	const float POWER = 64.f;

	float4 res_color = 0.f;
	
    Material mat;

	mat.diffuse = g_diffuse.Sample(g_sampler, input.tex_coor).rgb;
	mat.metallic = 0.f;
	mat.roughness = g_roughness.Sample(g_sampler, input.tex_coor).x;

	input.tbn_matrix[0].xyz = normalize(input.tbn_matrix[0].xyz);
	input.tbn_matrix[1].xyz = normalize(input.tbn_matrix[1].xyz);
	input.tbn_matrix[2].xyz = normalize(input.tbn_matrix[2].xyz);

	if(!isFrontFace)
	{
		input.tbn_matrix[0].xyz *= -1.f;
		input.tbn_matrix[2].xyz *= -1.f;
	}

	float3 light_vec;
	float3 view_vec = normalize(g_cameraPosition - input.world_position.xyz);
	float3 normal = normalize(mul(g_normals.Sample(g_sampler, input.tex_coor).xyz * 2.f - 1.f, input.tbn_matrix));
	
	
	
	res_color.rgb = calc_environment_light(view_vec, normal, mat) * g_ao.Sample(g_sampler, input.tex_coor).rgb;
	for (uint pLight_ind = 0; pLight_ind < g_lighting.pointLightNum; ++pLight_ind)
	{
		PointLight pointLight = g_lighting.pointLights[pLight_ind];
	 	light_vec = normalize(pointLight.position - input.world_position.xyz);
	
	 	float cosVL = saturate(dot(view_vec, -light_vec));
	 	float cosNL = dot(normal, light_vec);
	 	if(cosNL < 0)
	 	{
	 		res_color.rgb += g_translucency.Sample(g_sampler, input.tex_coor).rgb * pow(-cosNL, POWER) * cosVL * pointLight.radiance;
	 	} else
	 	{
	 		res_color.rgb += calc_point_light_pbr(pLight_ind, input.world_position.xyz, view_vec, input.tbn_matrix[2].xyz, normal, mat);
	 	}
	 		
	}
	
	for (uint sLight_ind = 0; sLight_ind < g_lighting.spotlightNum; ++sLight_ind)
	{
	 	Spotlight spotlight = g_lighting.spotlights[sLight_ind];
	 	light_vec = normalize(spotlight.position - input.world_position.xyz);
	
	 	float cosDSL = dot(spotlight.direction, -light_vec);
	 	float intensity = smoothstep(spotlight.outerCutOffCos, spotlight.cutOffCos, cosDSL);
	 	if (intensity == 0.f) continue;
	
	 	float cosVL = saturate(dot(view_vec, -light_vec));
	 	float cosNL = dot(normal, light_vec);

	 	if(cosNL < 0)
	 	{
	 		res_color.rgb += g_translucency.Sample(g_sampler, input.tex_coor).rgb * pow(-cosNL, POWER) * cosVL * spotlight.radiance;
	 	} else
	 	{
	 		 res_color.rgb += calc_spotlight_pbr(sLight_ind, input.world_position.xyz, view_vec, input.tbn_matrix[2].xyz, normal, mat);
	 	}
	}
	
    res_color.a = g_opacity.Sample(g_sampler, input.tex_coor).r;
	
    const float THRESHOLD = 0.1f;
    const uint FADING_PIXELS = 1u;
    float edge_distance = (res_color.a - THRESHOLD) / min(max(fwidth(res_color.a), 0.001f), 1.f);
    res_color.a = saturate(edge_distance / FADING_PIXELS);

	return res_color;
}