#include "light_calculation.hlsli"
#include "fullscreen.hlsli"
#include "octahedron_pack.hlsli"

struct vs_in
{
    float3 coor : Position;
    float2 texcoor : TexCoord;
    float3 normal : Normal;
    float3 tangent : Tangent;
    float3 bitangent : Bitangent;
    float3 radiance : Inst_Radiance;
    float radius : Inst_Radius;
    float3 position : Inst_Position;
    uint lightId : Inst_LightId;
};

struct vs_out
{
    float4 position : SV_Position;
    float3 view_vec : ViewVector;
    nointerpolation uint lightId : LightId;
    nointerpolation PointLight plight : PointLight;
};

vs_out main(vs_in input)
{
    vs_out res;
    
    float phi = 0.05f / max(input.radiance.x, max(input.radiance.y, input.radiance.z));
    float k = phi * 0.5f / PI;
    float rd = input.radius / sqrt(k * (2.f - k));
    res.position = mul(g_viewProj, float4((input.coor * rd + input.position), 1.f));
    res.view_vec = (g_cameraPosition - input.coor * rd - input.position);
    res.lightId = input.lightId;
    res.plight.position = input.position;
    res.plight.radiance = input.radiance;
    res.plight.radius = input.radius;
    res.plight.padding = rd;
    
    return res;
}

Texture2D g_emission : register(t5);
Texture2D g_normals : register(t6);
Texture2D g_albedo : register(t7);
Texture2D g_rmt : register(t8);
Texture2D g_depth : register(t9);

float4 ps_main(vs_out input) : SV_Target
{
    //return float4(input.plight.radiance / max(input.plight.radiance.x, max(input.plight.radiance.y, input.plight.radiance.z)), 1.f);
    
    const float POWER = 64.f;
    
    float4 res = float4(0.f, 0.f, 0.f, 1.f);
    float3 tex_coor = float3(input.position.xy, 0.f);
    Material mat;
	
    input.view_vec = normalize(input.view_vec);
	
    mat.diffuse = g_albedo.Load(tex_coor).rgb;
    mat.roughness = g_rmt.Load(tex_coor).r;
    mat.metallic = g_rmt.Load(tex_coor).g;
	
	//float3 normal = normals.Load(tex_coor).xyz * 2.f - 1.f;
	
    float3 normal = unpackOctahedron(g_normals.Load(tex_coor).xy);
    float3 mesh_normal = unpackOctahedron(g_normals.Load(tex_coor).zw);
	
    float dotViewCam = dot(
	normalize(g_frustum.bottom_left.xyz +
	          g_frustum.right_vector.xyz * 0.5 +
	          g_frustum.up_vector.xyz * 0.5),
	-input.view_vec
	);
	
    float sample_depth = world_depth_from_buffer(g_depth.Load(float3(input.position.xy, 0)).r);
    float3 world_pos = g_cameraPosition + -input.view_vec * sample_depth / dotViewCam;
	    
    if (length(world_pos - input.plight.position) > input.plight.padding)
    {
        discard;
        return 0.f;
    }
    
    float depth = 1;
    if (input.lightId < g_lighting.pointLightNum) 
        depth = point_shadow_calc(world_pos, input.plight.position, normal, input.lightId);
    res.rgb += depth * calc_point_light_pbr(input.plight, world_pos, input.view_vec, mesh_normal, normal, mat);
    
    float3 light_vec = normalize(input.plight.position - world_pos);
	
    float cosVL = saturate(dot(input.view_vec, -light_vec));
    float cosNL = dot(normal, light_vec);
    if (cosNL < 0)
    
    {
        res.rgb += mat.diffuse * g_rmt.Load(tex_coor).b * pow(-cosNL, POWER) * cosVL * input.plight.radiance;
    }
    
    return res;
}