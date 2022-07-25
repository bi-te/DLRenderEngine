#include "globals.hlsli"

struct vs_in
{
	float3 pos: POSITION;
	float2 tex_coords: TexCoord;
};

struct vs_out
{
	float4 position: SV_POSITION;
	float2 tex_coords: TexCoord;
};

cbuffer transformation: register(b1)
{
	matrix<float, 4, 4> transform;
}

vs_out main(float3 pos: Position, float2 tex_coords: TexCoord)
{
	vs_out res;
	res.tex_coords = tex_coords;
	res.position = mul(transform, float4(pos, 1.f));
	res.position = mul(g_viewProj, res.position);
	return res;
}

Texture2D object_texture;

float4 ps_main(vs_out input) : SV_TARGET
{
	return object_texture.Sample(g_sampler, input.tex_coords);
}