#include"globals.hlsli"

struct vs_in
{
    float3 position : Inst_Position;
    float angle : Inst_Angle;
    float2 size : Inst_Size;
    float thickness : Inst_Thickness;
    float2 tex_coor : Inst_TexCoor0;
    float frameFracTime : Inst_FrameFracTime;
    float2 tex_coor_next : Inst_TexCoor1;
    float3 tint : Inst_Tint;
    float alpha : Inst_Alpha;
    float2 image_size : Inst_ImageSize;
};

struct vs_out
{
    float3 world_pos : WorldPosition;
    float4 position : Sv_Position;
    float2 tex_coor : TexCoord0;
    nointerpolation float frameFracTime : FrameFracTime;
    nointerpolation float thickness : Thickness;
    float2 tex_coor_next : TexCoord1;
    nointerpolation float4 tint : Tint;
    float3x3 camera_rotation : CameraRotation;
};

vs_out main(uint index : Sv_VertexID, vs_in input)
{
    float sinA, cosA;
    sincos(radians(input.angle), sinA, cosA);
    float2x2 rotation = float2x2(
		 cosA, -sinA,
		 sinA, cosA
	);

    float3 x = X_VEC * input.size.x;
    float3 y = Y_VEC * input.size.y;

    float3 camera_right = normalize(g_frustum.right_vector).xyz;
    float3 camera_up = normalize(g_frustum.up_vector).xyz;
    float3 camera_front = normalize(cross(camera_right, camera_up));

    vs_out res;
    res.camera_rotation = float3x3(
		camera_right,
		camera_up,
		camera_front
	);
	
    res.position = float4(input.position, 1.f);

    res.tex_coor = input.tex_coor;
    float3 add_point = 0.f;

    res.frameFracTime = input.frameFracTime;
    res.tex_coor_next = input.tex_coor_next;

    switch (index)
    {
        case 0:
        case 3:
            add_point += -x - y;
            res.tex_coor.y += input.image_size.y;
            res.tex_coor_next.y += input.image_size.y;
            break;
        case 1:
            add_point += -x + y;
            break;
        case 2:
        case 4:
            add_point += +x + y;
            res.tex_coor.x += input.image_size.x;
            res.tex_coor_next.x += input.image_size.x;
            break;
        case 5:
            add_point += +x - y;
            res.tex_coor.xy += input.image_size;
            res.tex_coor_next.xy += input.image_size;
            break;
    }
    add_point.xy = mul(rotation, add_point.xy);
    res.position.xyz += mul(add_point, res.camera_rotation);

    res.world_pos = res.position.xyz;
    res.position = mul(g_viewProj, res.position);
    res.tint = float4(input.tint, input.alpha);
    res.thickness = input.thickness;
    return res;
}

Texture2D g_smoke_emva1 : register(t5);
Texture2D g_smoke_emission : register(t6);
Texture2D g_smoke_lightmap1 : register(t7);
Texture2D g_smoke_lightmap2 : register(t8);
Texture2D g_depth : register(t9);

void sampleUV(float frameFracTime, float2 uvThis, float2 uvNext, out float2 uv1, out float2 uv2)
{
    const float G_MV_SCALE = 0.001; // find such constant that frame transition becomes correct and smooth

    float2 mvA = g_smoke_emva1.Sample(g_sampler, uvThis).gb * 2.f - 1.f;
    float2 mvB = g_smoke_emva1.Sample(g_sampler, uvNext).gb * 2.f - 1.f;

    uv1 = uvThis; // this frame UV
    uv1 -= mvA * G_MV_SCALE * frameFracTime;

    uv2 = uvNext; // next frame UV
    uv2 -= mvB * G_MV_SCALE * (frameFracTime - 1.f);
}

float4 motion_sample(Texture2D tex, float2 uv1, float2 uv2, float fracTime)
{
    float4 valueA = tex.Sample(g_sampler, uv1);
    float4 valueB = tex.Sample(g_sampler, uv2);

    return lerp(valueA, valueB, fracTime);
}

float4 ps_main(vs_out input) : Sv_Target
{
    float2 uvA = 0.f, uvB = 0.f;
    sampleUV(input.frameFracTime, input.tex_coor, input.tex_coor_next, uvA, uvB);

    float4 res_color = 0.f;

    res_color.rgb = motion_sample(g_smoke_emission, uvA, uvB, input.frameFracTime).rgb * input.tint.rgb;
    res_color.w = motion_sample(g_smoke_emva1, uvA, uvB, input.frameFracTime).w * input.tint.w;

    float3 light1 = motion_sample(g_smoke_lightmap1, uvA, uvB, input.frameFracTime).xyz;
    float3 light2 = motion_sample(g_smoke_lightmap2, uvA, uvB, input.frameFracTime).xyz;

    for (uint lightInd = 0; lightInd < g_lighting.pointLightNum; lightInd++)
    {
        PointLight pLight = g_lighting.pointLights[lightInd];
        float3 light_vec = pLight.position - input.world_pos;
        float light_dist = length(light_vec);

        light_vec = normalize(light_vec);

        float lightMult = 0.f;
        float cosRight = dot(light_vec, input.camera_rotation[0].xyz);
        float cosUp = dot(light_vec, input.camera_rotation[1].xyz);
        float cosFront = dot(light_vec, input.camera_rotation[2].xyz);
        lightMult += abs(cosRight) * (cosRight > 0 ? light1.x : light1.y);
        lightMult += abs(cosUp) * (cosUp > 0 ? light1.z : light2.x);
        lightMult += abs(cosFront) * (cosFront > 0 ? light2.y : light2.z);

        float sphereCos = sqrt(light_dist * light_dist - pLight.radius * pLight.radius) / light_dist;
        float solidAngle = (1 - sphereCos) * 2.f * PI;
		
        res_color.rgb += input.tint.rgb * pLight.radiance * lightMult * solidAngle;
    }

    float sceneDepth = -g_near * g_far / (g_depth.Load(float3(input.position.xy, 0)).r * (g_near - g_far) - g_near);
    res_color.a *= saturate((sceneDepth - input.position.w) / input.thickness);
    return res_color;
}