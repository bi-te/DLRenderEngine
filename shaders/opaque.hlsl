#include "globals.hlsli"

struct vs_in
{
	float3 pos: Position;
	float2 tex_coords: TexCoord;
	float4x4 model_transform: Inst_ModelTransform;
};

struct vs_out
{
	float4 position: Sv_Position;
	float2 tex_coords: TexCoord;
};

float4x4 mesh_transform : register(b1);

vs_out main(vs_in input)
{
	vs_out res;
	res.tex_coords = input.tex_coords;
	res.position = mul(mesh_transform, float4(input.pos, 1.f));
	res.position = mul(input.model_transform, res.position);
	res.position = mul(g_viewProj, res.position);
	return res;
}

Texture2D object_texture;

float4 ps_main(vs_out input) : Sv_Target
{
	return object_texture.Sample(g_sampler, input.tex_coords);
}