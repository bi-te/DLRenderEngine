#include "light_calculation.hlsli"
#include "fullscreen.hlsli"
#include "octahedron_pack.hlsli"

struct vs_in
{
    uint index : SV_VertexID;
};

fullscreen_out main(vs_in input)
{
    return fullscreenVertex(input.index);
}

Texture2D g_emission : register(t5);
Texture2D g_normals : register(t6);
Texture2D g_albedo : register(t7);
Texture2D g_rm : register(t8);
Texture2D g_depth : register(t9);

float4 ps_main(fullscreen_out input) : SV_Target
{
    float4 res = float4(0.f, 0.f, 0.f, 1.f);
    float3 tex_coor = float3(input.pos.xy, 0.f);
    Material mat;
	
    input.cube_texcoord = normalize(input.cube_texcoord);
	
    mat.diffuse = g_albedo.Load(tex_coor).rgb;
    mat.roughness = g_rm.Load(tex_coor).r;
    mat.metallic = g_rm.Load(tex_coor).g;
	
    float3 normal = unpackOctahedron(g_normals.Load(tex_coor).xy);
	
    res.rgb += calc_environment_light(-input.cube_texcoord, normal, mat);
    res.rgb += g_emission.Load(tex_coor).rgb;
	
    return res;
}