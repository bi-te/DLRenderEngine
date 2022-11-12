#include "globals.hlsli"
#include "grass_helpers.hlsli"
#include "light_calculation.hlsli"
#include "octahedron_pack.hlsli"

struct vs_in
{
    float3 position : Inst_Position;
    float2 rel_position : Inst_RelPosition;
};

cbuffer GrassBuffer : register(b1)
{
    uint g_planes;
    uint g_sectors;
    float2 g_scale;
}

grass_out main(uint index : SV_VertexID, vs_in input)
{
    grass_properties gp;
    gp.position = input.position;
    gp.planes = g_planes;
    gp.sectors = g_sectors;
    gp.scale = g_scale;
    gp.rel_pos = input.rel_position;
    return grass_point(index, gp);
}

Texture2D g_diffuse : register(t5);
Texture2D g_normals : register(t6);
Texture2D g_roughness : register(t7);
Texture2D g_opacity : register(t8);
Texture2D g_translucency : register(t9);
Texture2D g_ao : register(t10);

struct ps_out
{
    float4 emissive : SV_Target0;
    float4 normals : SV_Target1;
    float4 albedo : SV_Target2;
    float4 rmt : SV_Target3;
};

ps_out ps_main(grass_out input, bool isFrontFace : SV_IsFrontFace)
{
    const float THRESHOLD = 0.1f;
    const float POWER = 64.f;
    const float TRANSMITTANCE = 0.3f;
	
    ps_out result;
	
    float alpha = g_opacity.Sample(g_sampler, input.tex_coor).r;
    if (alpha < THRESHOLD)
    {
        discard;
        return result;
    }

    result.albedo = g_diffuse.Sample(g_sampler, input.tex_coor);
    result.rmt.r = g_roughness.Sample(g_sampler, input.tex_coor).x;
    result.rmt.g = 0.f;
    result.rmt.b = TRANSMITTANCE;
    result.rmt.a = 0.f;
    result.emissive = 0.f;

    input.tbn_matrix[0].xyz = normalize(input.tbn_matrix[0].xyz);
    input.tbn_matrix[1].xyz = normalize(input.tbn_matrix[1].xyz);
    input.tbn_matrix[2].xyz = normalize(input.tbn_matrix[2].xyz);

    if (!isFrontFace)
    {
        input.tbn_matrix[0].xyz *= -1.f;
        input.tbn_matrix[2].xyz *= -1.f;
    }
	
    float3 mesh_normal = input.tbn_matrix[2].xyz;
    float3 normal = normalize(mul(g_normals.Sample(g_sampler, input.tex_coor).xyz * 2.f - 1.f, input.tbn_matrix));
	
    result.normals.xy = packOctahedron(normal);
    result.normals.zw = packOctahedron(mesh_normal);
	
    return result;
}