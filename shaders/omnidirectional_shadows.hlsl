#include "globals.hlsli"

struct vs_in
{
	float3 pos: Position;
	float2 tex_coords: TexCoord;
	float3 normal: Normal;
	float3 tangent: Tangent;
	float3 bitangent: Bitangent;
	float4x4 model_transform: Inst_ModelTransform;
	float3x3 model_scale: Inst_ModelScale;
};

struct vs_out
{
	float4 world_pos: Position;
};

cbuffer TransformBuffer: register(b1)
{
	float4x4 g_mesh_transform;
}

cbuffer LightTransBuffer: register(b2){
	uint g_index;
}

vs_out main(vs_in input) 
{
	vs_out res;
	res.world_pos = mul(g_mesh_transform, float4(input.pos, 1.f));
	res.world_pos = mul(input.model_transform, res.world_pos);
	return res;
}

struct gs_out
{
	float4 pos: Sv_Position;
	uint face_slice: Sv_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void gs_main(triangle vs_out input[3], inout TriangleStream<gs_out> output)
{
	gs_out res;
	[unroll]
	for (uint face = 0; face < 6u; ++face)
	{
		res.face_slice = g_index * 6u + face;

		for (uint vertex = 0; vertex < 3; vertex++)
		{
			res.pos = mul(g_lighting.pointTrans[g_index].light_view_proj[face], input[vertex].world_pos);
			output.Append(res);
		}
		output.RestartStrip();
	}
}

//struct ps_out
//{
//	float3 color: Sv_Target;
//	float depth : Sv_Depth;
//};
//
//ps_out ps_main(gs_out input){
//	float3 light_vec = g_lighting.pointLights[g_index].position - input.world_pos;
//	float dist = length(light_vec);
//
//	ps_out res;
//	res.color = float3(0.3f, 0.4f, 0.5f);
//	res.depth = (100.f - dist) / 100.f;
//
//	return res;
//}