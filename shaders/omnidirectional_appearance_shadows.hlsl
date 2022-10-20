#include "globals.hlsli"

struct vs_in
{
    float3 pos : Position;
    float2 tex_coords : TexCoord;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float3 bitangent : Bitangent;
    float4x4 model_transform : Inst_ModelTransform;
    float3x3 model_scale : Inst_ModelScale;
    float3 appearance_color : Inst_Color;
    float animationFract : Inst_AnimationFract;
};

struct vs_out
{
    float4 world_pos : Position;
    float2 texcoord : TexCoord;
    float animationFract : AnimationFract;
};

cbuffer TransformBuffer : register(b1)
{
    float4x4 g_mesh_transform;
}

cbuffer LightTransBuffer : register(b2)
{
    uint g_index;
}

vs_out main(vs_in input)
{
    vs_out res;
    res.world_pos = mul(g_mesh_transform, float4(input.pos, 1.f));
    res.world_pos = mul(input.model_transform, res.world_pos);
    res.animationFract = input.animationFract;
    res.texcoord = input.tex_coords;
    return res;
}

struct gs_out
{
    float4 pos : Sv_Position;
    float2 texcoord : TexCoord;
    float animationFract: AnimationFract;
    uint face_slice : Sv_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void gs_main(triangle vs_out input[3], inout TriangleStream<gs_out> output)
{
    gs_out res;
	[unroll]
    for (uint face = 0; face < 6u; ++face)
    {
        res.face_slice = g_index * 6u + face;
        res.animationFract = input[0].animationFract;
        for (uint vertex = 0; vertex < 3; vertex++)
        {
            res.pos = mul(g_lighting.pointTrans[g_index].light_view_proj[face], input[vertex].world_pos);
            res.texcoord = input[vertex].texcoord;
            output.Append(res);
        }
        output.RestartStrip();
    }
}

Texture2D g_noise : register(t9);

float ps_main(gs_out input) : SV_Depth
{
    float noiseTime = g_noise.Sample(g_sampler, input.texcoord).r;

    if (noiseTime > input.animationFract)
    {
        discard;
        return 0.f;
    }
    
    return input.pos.z;
}