#include "globals.hlsli"

struct grass_out
{
    float4 position : SV_Position;
    float4 world_position : WorldPosition;
    float2 tex_coor : TexCoord;
    float3x3 tbn_matrix : TBN_Matrix;
};

struct grass_properties
{
    float3 position;
    uint planes;
    uint sectors;
    float2 scale;
    float2 rel_pos;
};

float computeGrassAngle(float2 instancePos, float2 windDir) // windDir may be read from 2x2 wind matrix
{
    const float MAX_VARIATION = PI;
    float WIND_WAVE_LENGTH = 50;
    float POWER_WAVE_LENGTH = 233;
    const float WIND_OSCILLATION_FREQ = 0.666;
    const float POWER_OSCILLATION_FREQ = 1.0 / 8.0;

    float instanceRandom = frac(instancePos.x * 12345);
    float windCoord = dot(instancePos, windDir);
	
    float windPhaseVariation = instanceRandom * MAX_VARIATION;
    float windPhaseOffset = windCoord / WIND_WAVE_LENGTH + windPhaseVariation;
    float windOscillation = (sin(windPhaseOffset - WIND_OSCILLATION_FREQ * 2.f * PI * g_time) + 1) / 2;
	
    float powerPhaseOffset = windCoord / POWER_WAVE_LENGTH;
    float powerOscillation = (sin(powerPhaseOffset - POWER_OSCILLATION_FREQ * 2.f * PI * g_time) + 1) / 2;
	
    float minAngle = lerp(0.0, 0.3, powerOscillation);
    float maxAngle = lerp(0.1, 1.0, powerOscillation);
    return lerp(minAngle, maxAngle, windOscillation);
}

grass_out grass_point(uint index, grass_properties input)
{
    const uint VERTICES_PER_RECTANGLE = 6u;
    const float3 WIND_VECTOR = normalize(float3(5.f, 0.f, 1.f));
	
    uint plane_vertices = input.sectors * VERTICES_PER_RECTANGLE;
    float one_section_ratio = 1.f / input.sectors;

    float3 rel_pos = float3(input.rel_pos.x, 0.f, input.rel_pos.y);
    float wind_angle = computeGrassAngle(input.position.xz, WIND_VECTOR.xz);

    float3 wind_front = normalize(cross(WIND_VECTOR, Y_VEC));
    float3x3 wind_matrix = float3x3(
		WIND_VECTOR,
		Y_VEC,
		wind_front
	);

    float wind_angle_ratio = 0.f;

    float plane_angle = (index / plane_vertices) * PI / input.planes;
    float sinA, cosA;
    sincos(plane_angle, sinA, cosA);
    float3x3 rotation =
    {
        cosA, 0.f, -sinA,
		0.f, 1.f, 0.f,
		sinA, 0.f, cosA
    };

    grass_out res;
    res.world_position = float4(input.position, 1.f);
    res.tex_coor = 0.f;
    float3 add_point = 0.f;
    switch (index % 6u)
    {
        case (0):
        case (3):
            add_point += float3(0.5f, 0.f, 0.f);
            res.tex_coor += float2(0.75f, 0.f);
            wind_angle_ratio = 0.f;
            break;
        case (1):
            add_point += float3(0.5f, 0.f, 0.f);
            res.tex_coor += float2(0.75f, one_section_ratio);
            wind_angle_ratio = one_section_ratio;
            break;
        case (2):
        case (4):
            add_point += float3(-0.5f, 0.f, 0.f);
            res.tex_coor += float2(0.25f, one_section_ratio);
            wind_angle_ratio = one_section_ratio;
            break;
        case (5):
            add_point += float3(-0.5f, 0.f, 0.f);
            res.tex_coor += float2(0.25f, 0.f);
            wind_angle_ratio = 0.f;
            break;
    }
    add_point.x *= input.scale.x;
    add_point = mul(rotation, add_point);
	
    float3 world_tangle = mul(rotation, -X_VEC);
    float3 world_normal = mul(rotation, Z_VEC);

    res.tbn_matrix = float3x3(
		world_tangle.xyz,
		-Y_VEC,
		world_normal.xyz
	);

    float offset = input.scale.y / (wind_angle);
    float section_ratio = (index % plane_vertices) / VERTICES_PER_RECTANGLE * one_section_ratio;
    res.tex_coor.y = 1.f - res.tex_coor.y - section_ratio;
    wind_angle *= wind_angle_ratio + section_ratio;
	
    if (wind_angle > 0.f)
    {
        float sinW, cosW;
        sincos(wind_angle, sinW, cosW);
        float3x3 wind_rotation = float3x3(
			 cosW, sinW, 0.f,
			-sinW, cosW, 0.f,
			 0.f, 0.f, 1.f
		);

        add_point = mul(wind_matrix, add_point) - float3(offset, 0.f, 0.f);
        add_point = mul(transpose(wind_matrix), mul(wind_rotation, add_point) + float3(offset, 0.f, 0.f));

        res.tbn_matrix = mul(transpose(wind_matrix), mul(wind_rotation, mul(wind_matrix, res.tbn_matrix)));
    }

    res.world_position.xyz += add_point;
    res.position = mul(g_viewProj, res.world_position);

    return res;
}
