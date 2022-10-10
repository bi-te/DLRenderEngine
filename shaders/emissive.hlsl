#include "globals.hlsli"

struct vs_in
{
	float3 pos: Position;
	float2 tex_coords: TexCoord;
	float3 normal: Normal;
	float3 tangent: Tangent;
	float3 bitangent: Bitangent;
	float4x4 model_transform: Inst_ModelTransform;
	//float3x3 model_scale: Inst_ModelScale;
	float3 emissive_color: Inst_Color;
};

struct vs_out
{
	float4 position: Sv_Position;
	float4 world_position: WorldPosition;
	float3 emissive_color: Color;
	float3 normal: Normal;
};

cbuffer EmmisiveInstanceBuffer: register(b1)
{
	float4x4 g_mesh_transform;
}

vs_out main(vs_in input)
{
	vs_out res;
	res.world_position = mul(g_mesh_transform, float4(input.pos, 1.f));
	res.world_position = mul(input.model_transform, res.world_position);
	res.position = mul(g_viewProj, res.world_position);
	res.normal = mul(input.model_transform, mul(g_mesh_transform, float4(input.normal, 0.f))).xyz;
	res.emissive_color = input.emissive_color;
	return res;
}

float4 ps_main(vs_out input) : SV_Target
{
	float3 normal = normalize(input.normal);
	float3 cameraDir = normalize(g_cameraPosition - input.world_position.xyz);

	float3 normedEmission = input.emissive_color / max(input.emissive_color.x, max(input.emissive_color.y, max(input.emissive_color.z, 1.0)));

	float NoV = dot(cameraDir, normal);
	return float4(lerp(normedEmission * 0.33, input.emissive_color, pow(max(0.0, NoV), 4)), 1.f);
	//return float4(input.emissive_color, 1.f);
}