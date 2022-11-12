#include "light_calculation.hlsli"
#include "octahedron_pack.hlsli"

struct vs_in
{
	float3 pos : Position;
	float2 tex_coords : TexCoord;
	float3 normal : Normal;
	float3 tangent : Tangent;
	float3 bitangent : Bitangent;
	float4x4 model_transform : Inst_ModelTransform;
	float3x3 model_scale : Inst_ModelScale;
	float3 appearance_color : Inst_Color;
	float animaionStart : Inst_AnimationStart;
	float3 spherePosition : Inst_SpherePosition;
	float sphereRadiusPerSecond : Inst_SphereRadiusPerSecond;
};

struct vs_out
{
	float4 position : Sv_Position;
	float4 world_position : Position;
	float3x3 tbn_matrix : TBN_Matrix;
	float2 tex_coor : TexCoord;
	nointerpolation float3 appearance_color : Color;
	nointerpolation float sphereRadius : SphereRadius;
	nointerpolation float4 spherePos : SpherePos;
};

cbuffer TransformBuffer : register(b1)
{
	float4x4 g_mesh_transform;
}

vs_out main(vs_in input)
{
	vs_out res;
	res.tex_coor = input.tex_coords;
	res.appearance_color = input.appearance_color;
	res.sphereRadius = input.sphereRadiusPerSecond * (g_time - input.animaionStart);
	res.spherePos = float4(input.spherePosition, 1.f);
		
	float3 world_normal, world_tangent, world_bitangent;

	world_normal = mul(g_mesh_transform, float4(input.normal, 0.f)).rgb;
	world_normal = mul(input.model_scale, world_normal);

	world_tangent = mul(g_mesh_transform, float4(input.tangent, 0.f)).rgb;
	world_tangent = mul(input.model_scale, world_tangent);

	world_bitangent = mul(g_mesh_transform, float4(input.bitangent, 0.f)).rgb;
	world_bitangent = mul(input.model_scale, world_bitangent);    
	
	res.world_position = mul(g_mesh_transform, float4(input.pos, 1.f));
	res.world_position = mul(input.model_transform, res.world_position);
	
	res.position = mul(g_viewProj, res.world_position);

	res.tbn_matrix = float3x3(
		world_tangent.xyz,
		world_bitangent.xyz,
		world_normal.xyz
		);
	
	return res;
}

cbuffer MaterialBuffer : register(b2)
{
	Material g_material;
}

Texture2D g_diffuse : register(t5);
Texture2D g_normals : register(t6);
Texture2D g_roughness : register(t7);
Texture2D g_metallic : register(t8);
Texture2D g_noise : register(t9);

struct ps_out
{
	float4 emissive : SV_Target0;
	float4 normals : SV_Target1;
	float4 albedo : SV_Target2;
	float2 rm : SV_Target3;
};

ps_out ps_main(vs_out input)
{
	const float APPEARANCE_OFFSET = 0.1f;

	ps_out res;
	
	float noise_time = g_noise.Sample(g_sampler, input.tex_coor).r;
	
    if (length(input.world_position - input.spherePos) < input.sphereRadius)
    {
        discard;
        return res;
    }
    else if (length(input.world_position - input.spherePos) < input.sphereRadius + APPEARANCE_OFFSET)
    {
        res.rm = 0.f;
        res.albedo = 0.f;
        res.normals = 0.f;
        res.emissive = float4(input.appearance_color, 1.f);
        return res;
    }

	
	res.albedo.rgb = g_material.textures & MATERIAL_TEXTURE_DIFFUSE ? g_diffuse.Sample(g_sampler, input.tex_coor).rgb : g_material.diffuse;
	res.rm.g = g_material.textures & MATERIAL_TEXTURE_METALLIC ? g_metallic.Sample(g_sampler, input.tex_coor).x : g_material.metallic;
	res.rm.r = g_material.textures & MATERIAL_TEXTURE_ROUGHNESS ? g_roughness.Sample(g_sampler, input.tex_coor).x : g_material.roughness;
	res.emissive = 0.f;
	
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
	else
		normal = mesh_normal;
	
	//res.normals.xyz = res.normals.xyz / 2.f + 0.5f;
	res.normals.xy = packOctahedron(normal);
	res.normals.zw = packOctahedron(mesh_normal);
	
	return res;
}
	