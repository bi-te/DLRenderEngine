#include "globals.hlsli"
#include "dissolution_particle.hlsli"

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
	float3 position : Position;
	float3 dir : Direction;
    nointerpolation float sphereRadius : SphereRadius;
    nointerpolation float3 spherePosition : SpherePosition;
    nointerpolation float sphereRadiusPrev : SphereRadiusPrev;
    nointerpolation float3 color : COLOR;
};

cbuffer TransformBuffer : register(b1)
{
	float4x4 g_mesh_transform;
}

vs_out main(vs_in input)
{
   
	vs_out res;
	
	float sphereRadius = input.sphereRadiusPerSecond * (g_time - input.animaionStart);
	
	res.position = mul(g_mesh_transform, float4(input.pos, 1.f)).xyz;
	res.position = mul(input.model_transform, float4(res.position, 1.f));
	res.dir = mul(g_mesh_transform, float4(input.normal, 0.f)).rgb;
	res.dir = mul(input.model_scale, res.dir);
	res.spherePosition = input.spherePosition;
	res.sphereRadius = input.sphereRadiusPerSecond * (g_time - input.animaionStart);
	res.sphereRadiusPrev = input.sphereRadiusPerSecond * (g_time - g_frame_time - input.animaionStart);
    res.color = input.appearance_color;
	return res;
}

struct gs_out
{
	Particle particle : Particle;
	float4 spawned : SV_Position;
};

cbuffer ParticlesBuffer : register(b2)
{
    uint g_particlesBufferSize;
}


[maxvertexcount(4)]
void gs_main(triangle vs_out input[3], inout PointStream<gs_out> output)
{
    const float3 SIZE = float3(0.1f, 0.1f, 0.1f);
    gs_out res;
	
    res.particle.alpha = 1.f;
    res.particle.spawnTime = g_time;
    res.particle.size = SIZE;
    res.particle.color = input[0].color;
	
    for (uint vertex = 0; vertex < 3; vertex++)
    {
        res.particle.position += input[vertex].position / 3.f;
        res.particle.dir += input[vertex].dir / 3.f;
    }

    float sphereCenterPointLength = length(res.particle.position - input[0].spherePosition);
    if (input[0].sphereRadiusPrev < sphereCenterPointLength && sphereCenterPointLength <= input[0].sphereRadius)
    {
        res.spawned = float4(0.f, 0.f, 1.f, 1.f);
    }
    else
    {
        res.spawned = float4(10.f, 10.f, 1.f, 1.f);
    }
	
    output.Append(res);
    output.RestartStrip();
}

RWBuffer<uint> g_particlesRange : register(u0); //[0] - begin, [1] - count, [2] - deathcount;
RWStructuredBuffer<Particle> g_particles : register(u1);

void ps_main(gs_out input)
{
	uint prevCount;
	InterlockedAdd(g_particlesRange[1], 1, prevCount);
    uint index = (g_particlesRange[0] + prevCount) % g_particlesBufferSize;
    if (index >= g_particlesRange[0] && index < (g_particlesRange[0] + prevCount))
    {
        uint prevDeath;
        InterlockedAdd(g_particlesRange[2], 1, prevDeath);
    }
	g_particles[index] = input.particle;
}