#include "fullscreen.hlsli"

fullscreen_out main(uint index : SV_VertexID)
{
    return fullscreenVertex(index);
}

cbuffer MSAA : register(b1)
{
    uint g_msaa;
}

Texture2DMS<float> g_ms_depth : register(t5);

struct ps_out
{
    float depth : SV_Depth;
};

float ps_main(fullscreen_out input) : SV_Depth
{
    float min_depth = 0.f;
    for (uint sampleInd = 0; sampleInd < g_msaa; sampleInd++)
    {
        float sample_depth = g_ms_depth.Load(input.pos.xy, sampleInd).r;
        min_depth = max(min_depth, sample_depth);
    }

    return min_depth;
}