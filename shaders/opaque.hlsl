#include "light_calculation.hlsli"

struct vs_in
{
	float3 pos: Position;
	float2 tex_coords: TexCoord;
	float3 normal: Normal;
	float3 tangent: Tangent;
	float3 bitangent: Bitangent;
	float4x4 model_transform: Inst_ModelTransform;
	float3x3 model_scale: Inst_ModelScale;
};

struct vs_out
{
	float4 position: Sv_Position;
	float4 world_position: Position;
	float3x3 tbn_matrix: TBN_Matrix;
	float2 tex_coords: TexCoord;	
};

cbuffer TransformBuffer: register(b1)
{
	float4x4 g_mesh_transform;
}

vs_out main(vs_in input)
{
	vs_out res;
	res.tex_coords = input.tex_coords;

	res.world_position = mul(g_mesh_transform, float4(input.pos, 1.f));
	res.world_position = mul(input.model_transform, res.world_position);
	res.position = mul(g_viewProj, res.world_position);

	float3 world_normal, world_tangent, world_bitangent;

	//float4x4 meshToWorld = mul(g_mesh_transform, input.model_transform);

	//float3 axisX = normalize(float3(meshToWorld[0].x, meshToWorld[1].x, meshToWorld[2].x));
	//float3 axisY = normalize(float3(meshToWorld[0].y, meshToWorld[1].y, meshToWorld[2].y));
	//float3 axisZ = normalize(float3(meshToWorld[0].z, meshToWorld[1].z, meshToWorld[2].z));

	//world_normal = input.normal.x * axisX + input.normal.y * axisY + input.normal.z * axisZ;
	//world_tangent = input.tangent.x * axisX + input.tangent.y * axisY + input.tangent.z * axisZ;
	//world_bitangent = input.bitangent.x * axisX + input.bitangent.y * axisY + input.bitangent.z * axisZ;

	world_normal = mul(g_mesh_transform, float4(input.normal, 0.f)).rgb;
	world_normal = mul(input.model_scale, world_normal);

	world_tangent = mul(g_mesh_transform, float4(input.tangent, 0.f)).rgb;
	world_tangent = mul(input.model_scale, world_tangent);

	world_bitangent = mul(g_mesh_transform, float4(input.bitangent, 0.f)).rgb;
	world_bitangent = mul(input.model_scale, world_bitangent);

	res.tbn_matrix = float3x3(
		world_tangent.xyz,
		world_bitangent.xyz,
		world_normal.xyz
	);
	return res;
}

cbuffer MaterialBuffer: register(b2)
{
	Material g_material;
}

Texture2D g_diffuse : register(t5);
Texture2D g_normals: register(t6);
Texture2D g_roughness: register(t7);
Texture2D g_metallic: register(t8);

float3 ps_main(vs_out input) : Sv_Target
{
	Material mat;

	mat.diffuse = g_material.textures & MATERIAL_TEXTURE_DIFFUSE ? g_diffuse.Sample(g_sampler, input.tex_coords).rgb : g_material.diffuse;
	mat.metallic = g_material.textures & MATERIAL_TEXTURE_METALLIC ? g_metallic.Sample(g_sampler, input.tex_coords).x : g_material.metallic;
	mat.roughness = g_material.textures & MATERIAL_TEXTURE_ROUGHNESS ? g_roughness.Sample(g_sampler, input.tex_coords).x : g_material.roughness;

	input.tbn_matrix[0].xyz = normalize(input.tbn_matrix[0].xyz);
	input.tbn_matrix[1].xyz = normalize(input.tbn_matrix[1].xyz);
	input.tbn_matrix[2].xyz = normalize(input.tbn_matrix[2].xyz);
	
	float3 normal, mesh_normal = input.tbn_matrix[2].xyz;
	if (g_material.textures & MATERIAL_TEXTURE_NORMAL)
	{
		float3 texNormal = normalize(g_normals.Sample(g_sampler, input.tex_coords).xyz * 2.f - 1.f);
		texNormal.y *= g_material.textures & REVERSED_NORMAL_Y ? -1.f: 1.f;

		normal = normalize(mul(texNormal, input.tbn_matrix));
	}
	else normal = mesh_normal;

	float3 view_vec = normalize(g_cameraPosition - input.world_position.xyz);

	float3 res_color = calc_environment_light(view_vec, normal, mat);

	for (uint pLight_ind = 0; pLight_ind < g_lighting.pointLightNum; ++pLight_ind)
	{
		float depth = point_shadow_calc(input.world_position.xyz, mesh_normal, pLight_ind);
		res_color += depth * calc_point_light_pbr(input.world_position.xyz, view_vec, mesh_normal, normal, g_lighting.pointLights[pLight_ind], mat);
	}

	for(uint sLight_ind = 0; sLight_ind < g_lighting.spotlightNum; ++sLight_ind)
	{
		float depth = spot_shadow_calc(input.world_position.xyz, mesh_normal, sLight_ind);
		res_color += depth * calc_spotlight_pbr(input.world_position.xyz, view_vec, mesh_normal, normal, g_lighting.spotlights[sLight_ind], mat);
	}

	return res_color;
}
