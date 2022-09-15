#include "post_process_helpers.hlsli"
#include "fullscreen.hlsli"

vs_out main(uint index: SV_VertexID)
{
	return fullscreenVertex(index);
}

cbuffer PostProcessing: register(b1) {
	float g_ev100;
	uint g_msaa;
}
Texture2D g_hdr : register(t0);

float4 ps_main(vs_out input) : SV_Target
{
	float3 color = g_hdr.Load(input.pos.xyz).rgb;
	color = adjust_exposure(color, g_ev100);
	color = aces_tonemap(color);
	color = gamma_correction(color);

	return float4(color, 1.f);
}