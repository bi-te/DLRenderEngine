#include "globals.hlsli"

struct vs_out {
	float4 pos: SV_Position;
	float3 texcoord: TexCoord;
};

vs_out main(uint index: SV_VertexID)
{
	vs_out res;
	res.pos = float4(-1.f + 4.f * (index == 2), -1.f + 4.f * (index == 1), 0.f, 1.f);
	res.texcoord = frustum.bottom_left
		+ 2.f * (index == 2) * frustum.right_vector
		+ 2.f * (index == 1) * frustum.up_vector;

	return res;
}

TextureCube cubemap: register(t0);

float4 ps_main(vs_out input): SV_Target{
	return cubemap.Sample(g_sampler, input.texcoord);
}