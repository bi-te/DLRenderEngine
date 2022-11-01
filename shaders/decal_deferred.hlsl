#include "globals.hlsli"
#include "octahedron_pack.hlsli"

struct vs_in
{
    float3x3 decal_to_world : Inst_WorldToDecal;
    float3 world_position : Inst_WorldPosition;
    float3 size : Inst_Size;
    float rotation_angle : Inst_RotationAngle;
    float3 color : Inst_Color;
    uint objectId : Inst_ObjectId;
};

struct vs_out
{
    nointerpolation float4x4 worldToDecal : WorldToDecalMatrix;
    float4 position : SV_Position;
    float3 view_dir : ViewDir;
    uint id : MeshId;
    nointerpolation float3 color : Color;
    uint index : Index;
};

vs_out main(vs_in input, uint index : SV_VertexID)
{
    vs_out res;
	
    float3 cube_vertices[8] =
    {
        { -1.f, -1.f, -1.f }, //0
        { -1.f, 1.f, -1.f }, //1
        { 1.f, 1.f, -1.f }, //2
        { 1.f, -1.f, -1.f }, //3
        { -1.f, -1.f, 1.f }, //4
        { -1.f, 1.f, 1.f }, //5
        { 1.f, 1.f, 1.f }, //6
        { 1.f, -1.f, 1.f } //7
    };
	
	//back_faces
    uint cube_indices[36] =
    {
        3, 2, 1, 3, 1, 0, //z-
		4, 5, 6, 4, 6, 7, //z+
		0, 1, 5, 0, 5, 4, //x-
		7, 6, 2, 7, 2, 3, //x+
		7, 3, 0, 7, 0, 4, //y-
		2, 6, 5, 2, 5, 1 //y+
    };
	
	
    float sinA, cosA;
    sincos(radians(input.rotation_angle), sinA, cosA);
	
    float3x3 basisRotation = float3x3(
		 cosA, -sinA, 0.f,
		 sinA, cosA, 0.f,
		 0.f, 0.f, 1.f
	);
	
    res.position = float4(input.world_position, 1.f);
	
    input.decal_to_world = mul(input.decal_to_world, float3x3(
		 cosA, -sinA, 0.f,
		 sinA, cosA, 0.f,
		 0.f, 0.f, 1.f
	));
	
    res.worldToDecal = 0.f;
    res.worldToDecal[0].xyz = input.decal_to_world._11_21_31 / pow(input.size.x, 2.f);
    res.worldToDecal[1].xyz = input.decal_to_world._12_22_32 / pow(input.size.y, 2.f);
    res.worldToDecal[2].xyz = input.decal_to_world._13_23_33 / pow(input.size.z, 2.f);
    res.worldToDecal._14_24_34_44 = float4(
		-dot(input.world_position, res.worldToDecal[0].xyz),
		-dot(input.world_position, res.worldToDecal[1].xyz),
		-dot(input.world_position, res.worldToDecal[2].xyz),
		1.f
	);
	
    res.index = index;
	
    res.position.xyz += mul(input.decal_to_world, cube_vertices[cube_indices[index]]);
    res.view_dir = res.position.xyz - g_cameraPosition;
    res.position = mul(g_viewProj, res.position);
	
    res.color = input.color;
    res.id = input.objectId;
    return res;
}

struct ps_out
{
    float4 normals : SV_Target1;
    float4 albedo : SV_Target2;
};

Texture2D g_decal : register(t5);
Texture2D g_depth : register(t6);
Texture2D g_normals : register(t7);
Texture2D<uint> g_ids : register(t8);

ps_out ps_main(vs_out input)
{
    ps_out res;
	
    float dotViewCam = dot(
	normalize(g_frustum.bottom_left.xyz +
	g_frustum.right_vector.xyz * 0.5 +
	g_frustum.up_vector.xyz * 0.5),
	normalize(input.view_dir)
	);
	
    float sample_depth = -g_near * g_far / (g_depth.Load(float3(input.position.xy, 0)).r * (g_near - g_far) - g_near);
	
    float3 sample_world_pos = g_cameraPosition + normalize(input.view_dir) * sample_depth / dotViewCam;
    float3 sample_decal_pos = mul(input.worldToDecal, float4(sample_world_pos, 1.f));
	
    float4 decal_sample = g_decal.Sample(g_sampler, sample_decal_pos.xy / 2.f + 0.5f);
    decal_sample.xyz = decal_sample.xyz * 2.f - 1.f;
	
    uint objectId = g_ids.Load(int3(input.position.xy, 0)).x;
	
    if (sample_decal_pos.x < -1 || sample_decal_pos.x > 1 ||
		sample_decal_pos.y < -1 || sample_decal_pos.y > 1 ||
		sample_decal_pos.z < -1 || sample_decal_pos.z > 1)
    {
        switch (input.index / 6)
        {
            case 0:
                res.albedo = float4(10.f, 0.f, 0.f, 1.f);
                break;
            case 1:
                res.albedo = float4(10.f, 10.f, 0.f, 1.f);
                break;
            case 2:
                res.albedo = float4(0.f, 10.f, 0.f, 1.f);
                break;
            case 3:
                res.albedo = float4(10.f, 10.f, 10.f, 1.f);
                break;
            case 4:
                res.albedo = float4(0.f, 0.f, 10.f, 1.f);
                break;
            case 5:
                res.albedo = float4(10.f, 0.f, 10.f, 1.f);
                break;
        }
		
        discard;
        return res;
    }
    else if (decal_sample.a < 0.9 || objectId != input.id)
    {
        discard;
		//res.albedo = float4(0.f, 0.f, 0.f, 1.f);
        return res;
    }
	
	
    float4 normal_sample = g_normals.Load(float3(input.position.xy, 0));
    float3 normal = unpackOctahedron(normal_sample.xy);
    float3 tangent = normalize(normalize(input.worldToDecal[0].xyz) - normal * dot(normal, normalize(input.worldToDecal[0].xyz)));
    float3 bitangent = cross(normal, tangent);
    float3 res_normal = decal_sample.x * tangent + decal_sample.y * bitangent + decal_sample.z * normal;
	
    res.albedo = float4(input.color, 1.f);
    res.normals.xy = packOctahedron(normalize(res_normal + normal));
    res.normals.zw = normal_sample.zw;
    return res;
}