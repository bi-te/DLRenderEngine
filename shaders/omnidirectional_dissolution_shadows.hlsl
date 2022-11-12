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
    float animaionStart : Inst_AnimationStart;
    float3 spherePosition : Inst_SpherePosition;
    float sphereRadiusPerSecond : Inst_SphereRadiusPerSecond;
};

struct vs_out
{
    float4 world_pos : Position;
    nointerpolation float sphereRadius : SphereRadius;
    nointerpolation float4 spherePos : SpherePos;
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
    res.sphereRadius = input.sphereRadiusPerSecond * (g_time - input.animaionStart);
    res.spherePos = float4(input.spherePosition, 1.f);
    return res;
}

struct gs_out
{
    float4 pos : Sv_Position;
    float4 world_position : WorldPosition;
    nointerpolation float4 spherePos : SpherePos;
    nointerpolation float sphereRadius : SphereRadius;
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
        res.spherePos = input[0].spherePos;
        res.sphereRadius = input[0].sphereRadius;
        for (uint vertex = 0; vertex < 3; vertex++)
        {
            res.world_position = input[vertex].world_pos;
            res.pos = mul(g_lighting.pointTrans[g_index].light_view_proj[face], input[vertex].world_pos);
            output.Append(res);
        }
        output.RestartStrip();
    }
}

float ps_main(gs_out input) : SV_Depth
{
    if (length(input.world_position - input.spherePos) < input.sphereRadius)
    {
        discard;
        return 0.f;
    }
    
    return input.pos.z;
}