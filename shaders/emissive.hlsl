#include "globals.hlsli"

struct vs_in
{
	float3 pos: Position;
	float4x4 model_transform: Inst_Transform;
	float3 emissive_color: Inst_Color;
};

struct vs_out
{
	float4 pos: Sv_Position;
	float3 emissive_color: Inst_Color;
};

cbuffer EmmisiveInstanceBuffer: register(b1)
{
	float4x4 mesh_transform;
}

vs_out main(vs_in input)
{
	vs_out res;
	res.pos = mul(mesh_transform, float4(input.pos, 1.f));
	res.pos = mul(input.model_transform, res.pos);
	res.pos = mul(g_viewProj, res.pos);
	res.emissive_color = input.emissive_color;
	return res;
}

float4 ps_main(vs_out input) : SV_Target
{
	return float4(input.emissive_color, 1.f);
}