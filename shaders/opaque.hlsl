#include "light_calculation.hlsli"

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
		world_tangent.xyz,
		world_bitangent.xyz,
		world_normal.xyz
	);
	return res;
}

float3 ps_main(vs_out input) : Sv_Target
{
	Material mat;

	mat.diffuse = material.textures & MATERIAL_TEXTURE_DIFFUSE ? diffuse.Sample(g_sampler, input.tex_coords) : material.diffuse;
	mat.metallic = material.textures & MATERIAL_TEXTURE_METALLIC ? metallic.Sample(g_sampler, input.tex_coords) : material.metallic;
	mat.roughness = material.textures & MATERIAL_TEXTURE_ROUGHNESS ? roughness.Sample(g_sampler, input.tex_coords) : material.roughness;

	input.tbn_matrix[0].xyz = normalize(input.tbn_matrix[0].xyz);
	input.tbn_matrix[1].xyz = normalize(input.tbn_matrix[1].xyz);
	input.tbn_matrix[2].xyz = normalize(input.tbn_matrix[2].xyz);
	
	float3 normal, mesh_normal = input.tbn_matrix[2].xyz;
	if (material.textures & MATERIAL_TEXTURE_NORMAL)
	{
		float3 texNormal = normals.Sample(g_sampler, input.tex_coords) * 2.f - 1.f;
		texNormal.y *= material.textures & REVERSED_NORMAL_Y ? -1.f : 1.f;

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
