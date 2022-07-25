#include "globals.hlsli"

struct vs_out {
	float4 pos: SV_Position;
	float3 texcoord: TexCoord;
};

cbuffer frustrum: register(b1)
{
	float3 bottom_left;
	float3 up_vector;
	float3 right_vector;
}

vs_out main(uint index: SV_VertexID)
{
	vs_out res;
	res.pos = float4(-1.f + 4.f * (index == 2), -1.f + 4.f * (index == 1), 0.f, 1.f);
	res.texcoord = bottom_left + 2.f * (index == 2) * right_vector + 2.f * (index == 1) * up_vector;
	return res;
}

TextureCube cubemap: register(t0);

float4 ps_main(vs_out input): SV_Target{
	return cubemap.Sample(g_sampler, input.texcoord);
}