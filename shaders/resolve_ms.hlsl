#include "post_process_helpers.hlsli"
#include "fullscreen.hlsli"

vs_out main(uint index: SV_VertexID)
{
	return fullscreenVertex(index);
}

cbuffer PostProcessing: register(b1){
	float g_ev100;
	uint g_msaa;
}
Texture2DMS<float4> g_hdr: register(t0);

float4 ps_main(vs_out input) : SV_Target
{
	float3 colorSum = 0.f;
	for (uint i = 0; i < g_msaa; ++i)
	{
		float3 color= g_hdr.Load(input.pos.xy, i).rgb;
		color = adjust_exposure(color, g_ev100);
		color = aces_tonemap(color);
		colorSum += color;
	}

	colorSum /= g_msaa;
	colorSum = gamma_correction(colorSum);

	return float4(colorSum, 1.f);
}