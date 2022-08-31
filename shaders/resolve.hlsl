#include "post_process_helpers.hlsli"
#include "fullscreen.hlsli"

vs_out main(uint index: SV_VertexID)
{
	return fullscreenVertex(index);
}

cbuffer PostProcessing: register(b1){
	float g_ev100;
}
Texture2D g_hdr: register(t0);

float4 ps_main(vs_out input) : SV_Target
{
	float4 color = g_hdr.Load(int3(input.pos.x, input.pos.y, 0));

	color.xyz = adjust_exposure(color.xyz, g_ev100);
	color.xyz = aces_tonemap(color.xyz);
	color.xyz = gamma_correction(color.xyz);

	return color;
}
