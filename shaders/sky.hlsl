#include "globals.hlsli"
#include "fullscreen.hlsl"

TextureCube cubemap: register(t0);

float4 ps_main(vs_out input): SV_Target{
	return cubemap.Sample(g_sampler, input.texcoord);
}