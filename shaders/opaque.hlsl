#include "globals.hlsli"
#include "pbr_render.hlsli"

struct vs_in
{
	float3 pos: Position;
	float2 tex_coords: TexCoord;
	float3 normal: Normal;
	float3 tangent: Tangent;
	float3 bitangent: Bitangent;
	float4x4 model_transform: Inst_ModelTransform;
	float3 model_scale: Inst_ModelScale;
};

struct vs_out
{
	float4 position: Sv_Position;
	float4 world_position: Position;
	float3x3 tbn_matrix: TBN_Matrix;
	float2 tex_coords: TexCoord;	
};

cbuffer TransformBuffer: register(b2)
{
	float4x4 mesh_transform;
}

cbuffer MaterialBuffer: register(b3)
{
	Material material;
}

Texture2D diffuse: register(t0);
Texture2D normals: register(t1);
Texture2D roughness: register(t2);
Texture2D metallic: register(t3);

vs_out main(vs_in input)
{
	vs_out res;
	res.tex_coords = input.tex_coords;

	res.world_position = mul(mesh_transform, float4(input.pos, 1.f));
	res.world_position = mul(input.model_transform, res.world_position);
	res.position = mul(g_viewProj, res.world_position);

	float3 world_normal, world_tangent, world_bitangent;

	world_normal = mul(mesh_transform, float4(input.normal, 0.f));
	world_normal = mul(input.model_transform, world_normal) / input.model_scale;

	world_tangent = mul(mesh_transform, float4(input.tangent, 0.f));
	world_tangent = mul(input.model_transform, world_tangent) / input.model_scale;

	world_bitangent = mul(mesh_transform, float4(input.bitangent, 0.f));
	world_bitangent = mul(input.model_transform, world_bitangent) / input.model_scale;

	res.tbn_matrix = float3x3(
		normalize(world_tangent.xyz),
		normalize(world_bitangent.xyz),
		normalize(world_normal.xyz));
	return res;
}


float3 calc_direct_light_pbr(float3 view_vec, float3 mesh_normal, float3 normal, DirectLight dirlight, Material mat);
float3 calc_point_light_pbr(float3 position, float3 view_vec, float3 mesh_normal, float3 normal, PointLight pointLight, Material material);
float3 calc_spotlight_pbr(float3 position, float3 view_vec, float3 mesh_normal, float3 normal, Spotlight spotlight, Material material);


float3 ps_main(vs_out input) : Sv_Target
{
	Material mat;

	mat.diffuse = material.hasDiffuseTexture ? diffuse.Sample(g_sampler, input.tex_coords) : material.diffuse;
	mat.metallic = material.hasMetallicTexture ? metallic.Sample(g_sampler, input.tex_coords) : material.metallic;
	mat.roughness = material.hasRoughnessTexture ? roughness.Sample(g_sampler, input.tex_coords) : material.roughness;

	
	float3 normal, mesh_normal = normalize(float3(input.tbn_matrix[2].x, input.tbn_matrix[2].y, input.tbn_matrix[2].z));;
	if (material.hasNormalsTexture)
	{
		float3 texNormal = normals.Sample(g_sampler, input.tex_coords) * 2.f - 1.f;

		if (material.reverseNormalTextureY)
			texNormal.y *= -1.f;

		normal = normalize(mul(texNormal, input.tbn_matrix));
	}

	else normal = mesh_normal;

	float3 view_vec = normalize(g_cameraPosition - input.world_position);

	float3 res_color = mat.diffuse * ambient;

	res_color += calc_direct_light_pbr(view_vec, mesh_normal, normal, dirLight, mat);

	for (int pLight_ind = 0; pLight_ind < pointLightNum; ++pLight_ind)
		res_color += calc_point_light_pbr(input.world_position, view_vec, mesh_normal, normal, pointLights[pLight_ind], mat);

	for (int sLight_ind = 0; sLight_ind < spotlightNum; ++sLight_ind)
		res_color += calc_spotlight_pbr(input.world_position, view_vec, mesh_normal, normal, spotlights[sLight_ind], mat);

	return res_color;
}

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

	float cosDSL = dot(spotlight.direction , -light_vec);
	float intensity = smoothstep(spotlight.outerCutOff, spotlight.cutOff, cosDSL);
	if (intensity == 0.f) return 0.f;

	float cosPhi = sqrt(dist * dist - spotlight.radius * spotlight.radius) / dist;
	float attenuation = (1.f - cosPhi) * intensity;

	float3 closest_vec = closest_sphere_direction(light_vec * dist, light_vec,
		reflect(-view_vec, normal), dist, spotlight.radius, cosPhi);
	clamp_to_horizon(normal, closest_vec, 0.01f);

	return cook_torrance_aprox(light_vec, closest_vec, normal, view_vec, spotlight.radiance, attenuation, material);
}
