#include "globals.hlsli"
#include "fullscreen.hlsli"

TextureCube cubemap : register(t5);

fullscreen_out main(uint index : SV_VertexID)
{
    return fullscreenVertex(index);
}

float4 ps_main(fullscreen_out input) : SV_Target
{
    return cubemap.Sample(g_sampler, input.cube_texcoord);
}