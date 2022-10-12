#include "globals.hlsli"

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
	float4 pos: Sv_Position;
	nointerpolation uint slice: Sv_RenderTargetArrayIndex;
};

cbuffer TransformBuffer: register(b1)
{
	float4x4 g_mesh_transform;
}

cbuffer LightTransBuffer : register(b2) {
	uint g_index;
}

vs_out main(vs_in input)
{
	vs_out res;
	res.pos = mul(g_mesh_transform, float4(input.pos, 1.f));
	res.pos = mul(input.model_transform, res.pos);
	res.pos = mul(g_lighting.spotTrans[g_index].light_view_proj, res.pos);
	res.slice = g_index;
	return res;
}


