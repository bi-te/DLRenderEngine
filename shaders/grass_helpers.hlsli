#include "globals.hlsli"

struct grass_out
{
	float4 position: SV_Position;
	float4 world_position: WorldPosition;
	float2 tex_coor: TexCoord;
	float3x3 tbn_matrix: TBN_Matrix;
};

struct grass_properties
{
	float3 position;
	uint planes;
	uint sectors;
	float2 scale;
};

grass_out grass_point(uint index, grass_properties input) 
{
	const uint VerticesPerTriangle = 6u;
	uint plane_vertices = input.sectors * VerticesPerTriangle;
	float one_section_ratio = 1.f / input.sectors;

	const float3 wind_vector = normalize(float3(-1.f, 0.f, 0.f));
	const float wind_power = pow(cos(g_time), 2.f);

	float3 wind_front = normalize(cross(wind_vector, Y_VEC));
	float3x3 wind_matrix = float3x3(
		wind_vector,
		Y_VEC,
		wind_front
	);

	float wind_angle = 0.f;

	float plane_angle = (index / plane_vertices) * PI / input.planes;
	float sinA, cosA;
	sincos(plane_angle, sinA, cosA);
	float3x3 rotation = {
		cosA,  0.f, -sinA,
		0.f,   1.f,  0.f,
		sinA,  0.f,  cosA
	};

	grass_out res;
	res.world_position = float4(input.position, 1.f);
	res.tex_coor = 0.f;
	float3 add_point = 0.f;
	switch (index % 6u)
	{
		case(0):
		case(3): 
			add_point += float3( 0.5f, 0.f, 0.f);
			res.tex_coor += float2(0.75f, 0.f);
			wind_angle = 0.f;
			break;
		case(1): 
			add_point += float3( 0.5f, 0.f, 0.f);
			res.tex_coor += float2(0.75f, one_section_ratio);
			wind_angle = one_section_ratio;
			break;
		case(2): 
		case(4): 
			add_point += float3(-0.5f, 0.f, 0.f);
			res.tex_coor += float2(0.25f, one_section_ratio);
			wind_angle = one_section_ratio;
			break;
		case(5): 
			add_point += float3(-0.5f, 0.f, 0.f);
			res.tex_coor += float2(0.25f, 0.f);
			wind_angle = 0.f;
			break;
	}
	add_point.x *= input.scale.x;
	add_point = mul(rotation, add_point);
	
	float3 world_tangle = mul(rotation, -X_VEC);
	float3 world_normal = mul(rotation,  Z_VEC);

	res.tbn_matrix = float3x3(
		world_tangle.xyz,
		-Y_VEC,
		world_normal.xyz
	);

	wind_angle += (index % plane_vertices) / VerticesPerTriangle * one_section_ratio;
	res.tex_coor.y = 1.f - res.tex_coor.y - (index % plane_vertices) / VerticesPerTriangle * one_section_ratio;

	float offset = input.scale.y / (wind_power * 0.5f * PI);
	if(wind_angle > 0.f)
	{
		wind_angle *= wind_power * 0.5f * PI;
		
		float sinW, cosW;
		sincos(wind_angle, sinW, cosW);
		float3x3 wind_rotation = float3x3(
			 cosW,  sinW, 0.f,
			-sinW,  cosW, 0.f,
			 0.f,   0.f,  1.f  
		);

		add_point = mul(wind_matrix, add_point) - float3(offset, 0.f, 0.f);
		add_point = mul(transpose(wind_matrix), mul(wind_rotation, add_point) + float3(offset, 0.f, 0.f));

		res.tbn_matrix = mul(transpose(wind_matrix), mul(wind_rotation, mul(wind_matrix, res.tbn_matrix)));
	}

	res.world_position.xyz += add_point;
	res.position = mul(g_viewProj, res.world_position);

	return res;
}
