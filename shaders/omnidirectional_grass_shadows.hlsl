#include "globals.hlsli"
#include "grass_helpers.hlsli"

struct vs_in
{
	float3 position: Inst_Position;
};

cbuffer GrassBuffer: register(b1)
{
	uint g_planes;
	uint g_sectors;
	float2 g_scale;
}

cbuffer LightTransBuffer: register(b2){
	uint g_index;
}

grass_out main(uint index: SV_VertexID, vs_in input) 
{
		grass_properties gp;
	gp.position = input.position;
	gp.planes = g_planes;
	gp.sectors = g_sectors;
	gp.scale = g_scale;
	return grass_point(index, gp);
}

struct gs_out
{
	float4 pos: Sv_Position;
	float2 tex_coor: TexCoor;
	uint face_slice: Sv_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void gs_main(triangle grass_out input[3], inout TriangleStream<gs_out> output)
{
	gs_out res;
	[unroll]
	for (uint face = 0; face < 6u; ++face)
	{
		res.face_slice = g_index * 6u + face;
		for (uint vertex = 0; vertex < 3; vertex++)
		{
			res.tex_coor = input[vertex].tex_coor;
			res.pos = mul(g_lighting.pointTrans[g_index].light_view_proj[face], input[vertex].world_position);
			output.Append(res);
		}
		output.RestartStrip();
	}
}

Texture2D g_opacity: register(t5);

float ps_main(gs_out input): Sv_Depth
{
	if(g_opacity.Sample(g_sampler, input.tex_coor).r < 0.001f)
	{
		discard;
		return 0.f;
	}
	return input.pos.z;
}