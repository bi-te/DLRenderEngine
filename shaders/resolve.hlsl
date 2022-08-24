#include "post_process_helpers.hlsli"
struct vs_out {
	float4 pos: SV_Position;
	float2 texcoord: TexCoord;
};

vs_out main(uint index: SV_VertexID)
{
	vs_out res;
	res.pos = float4(-1.f + 4.f * (index == 1), 1.f - 4.f * (index == 2), 0.f, 1.f);
	res.texcoord = float2(2.f * (index == 1), 2.f * (index == 2));

	return res;
}

cbuffer PostProcessing: register(b1){
	float ev100;
}
Texture2D hdr: register(t0);

float4 ps_main(vs_out input) : SV_Target
{
	int width, height;
	hdr.GetDimensions(width, height);
	float4 color = hdr.Load(int3(input.texcoord.x * width, input.texcoord.y * height, 0));

	color.xyz = adjust_exposure(color.xyz, ev100);
	color.xyz = aces_tonemap(color.xyz);
	color.xyz = gamma_correction(color.xyz);

	return color;
}
