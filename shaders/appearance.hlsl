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
	float3 appearance_color: Inst_Color;
	float animationFract : Inst_AnimationFract;
};

struct vs_out
{
	float4 position: Sv_Position;
	float4 world_position: Position;
	float3x3 tbn_matrix: TBN_Matrix;
	float2 tex_coor: TexCoord;
	float animationFract : AnimationFract;
	float3 appearance_color: Color;
};

cbuffer TransformBuffer: register(b1)
{
	float4x4 g_mesh_transform;
}

vs_out main(vs_in input)
{
	vs_out res;
	res.tex_coor = input.tex_coords;
	res.appearance_color = input.appearance_color;

	res.world_position = mul(g_mesh_transform, float4(input.pos, 1.f));
	res.world_position = mul(input.model_transform, res.world_position);
	res.position = mul(g_viewProj, res.world_position);
	res.animationFract = input.animationFract;

	float3 world_normal, world_tangent, world_bitangent;

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
Texture2D g_noise: register(t9);

float calcMipLevel(float2 tex_coor)
{
	float2 dx = ddx(tex_coor);
	float2 dy = ddy(tex_coor);
	float delta_max_sqr = max(dot(dx, dx), dot(dy, dy));

	return max(0.f, 0.5f * log2(delta_max_sqr));
}

float4 ps_main(vs_out input) : Sv_Target
{
	Material mat;

	mat.diffuse = g_material.textures & MATERIAL_TEXTURE_DIFFUSE ? g_diffuse.Sample(g_sampler, input.tex_coor).rgb : g_material.diffuse;
	mat.metallic = g_material.textures & MATERIAL_TEXTURE_METALLIC ? g_metallic.Sample(g_sampler, input.tex_coor).x : g_material.metallic;
	mat.roughness = g_material.textures & MATERIAL_TEXTURE_ROUGHNESS ? g_roughness.Sample(g_sampler, input.tex_coor).x : g_material.roughness;

	input.tbn_matrix[0].xyz = normalize(input.tbn_matrix[0].xyz);
	input.tbn_matrix[1].xyz = normalize(input.tbn_matrix[1].xyz);
	input.tbn_matrix[2].xyz = normalize(input.tbn_matrix[2].xyz);

	float3 normal, mesh_normal = input.tbn_matrix[2].xyz;
	if (g_material.textures & MATERIAL_TEXTURE_NORMAL)
	{
		float3 texNormal = normalize(g_normals.Sample(g_sampler, input.tex_coor).xyz * 2.f - 1.f);
		texNormal.y *= g_material.textures & REVERSED_NORMAL_Y ? -1.f : 1.f;

		normal = normalize(mul(texNormal, input.tbn_matrix));
	}
	else normal = mesh_normal;

	//if (g_noise.Sample(g_sampler, input.tex_coor).r < input.animationFract)
	//{
	//	discard;
	//	return float4(0.f, 0.f, 0.f, 0.0f);
	//}

	if(g_noise.Sample(g_sampler, input.tex_coor).r > input.animationFract)
	{

		return float4(input.appearance_color, 0.5f);
	}

	float3 view_vec = normalize(g_cameraPosition - input.world_position.xyz);

	float3 res_color = calc_environment_light(view_vec, normal, mat);
	res_color += calc_point_lights_pbr(input.world_position.xyz, view_vec, mesh_normal, normal, mat);
	res_color += calc_spotlights_pbr(input.world_position.xyz, view_vec, mesh_normal, normal, mat);

	return float4(res_color, 1.f);
}
	