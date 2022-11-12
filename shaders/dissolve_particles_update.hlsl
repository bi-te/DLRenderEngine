#include "globals.hlsli"
#include "dissolution_particle.hlsli"
#include "octahedron_pack.hlsli"

RWBuffer<uint> g_particlesRange : register(u0); //[0] - begin, [1] - count, [2] - deathcount;
RWStructuredBuffer<Particle> g_particles : register(u1);

cbuffer ParticlesSizeBuffer : register(b1)
{
	uint g_particlesBufferSize;
}

Texture2D g_normals : register(t5);
Texture2D g_depth : register(t6);

[numthreads(64, 1, 1)]
void cs_main( uint3 dtId : SV_DispatchThreadID )
{
    const float SLOWING = 0.75f;
	
	if (dtId.x > g_particlesBufferSize || dtId.x >= g_particlesRange[1])
		return;
	uint index = (g_particlesRange[0] + dtId.x) % g_particlesBufferSize; 

    Particle particle = g_particles[index];
    particle.alpha = particle_fading(g_time - particle.spawnTime, MAX_PARTICLE_LIFETIME);
    
    float3 v = particle.dir;
    float3 a = -Y_VEC * SMALL_G;
    float t = g_frame_time;
    particle.position += v * t + a * pow(t, 2) / 2.f;
    particle.dir += a * t;
    
    float4 pos = mul(g_viewProj, float4(particle.position, 1.f));
    pos.xy = pos.xy / (2.f * pos.w) + 0.5f;
    pos.y = 1.f - pos.y;
    
    float3 sampledNormal = unpackOctahedron(g_normals.SampleLevel(g_linear_clamp_sampler, pos.xy, 0).xy);
    float sampledDepth = world_depth_from_buffer(g_depth.SampleLevel(g_linear_clamp_sampler, pos.xy, 0).r);
	
    float3 view_vec = normalize(g_frustum.bottom_left.xyz +
	g_frustum.right_vector.xyz * pos.x +
	g_frustum.up_vector.xyz * (1.f - pos.y));
	
    float3 cam_vec = normalize(g_frustum.bottom_left.xyz +
	g_frustum.right_vector.xyz * 0.5 +
	g_frustum.up_vector.xyz * 0.5);
    
    float dotViewCam = dot(view_vec, cam_vec);
	
    float3 sample_world_pos = g_cameraPosition + normalize(view_vec) * sampledDepth / dotViewCam;
    float radius = length(particle.position - sample_world_pos);
    if (radius < particle.size.z)
    {
        particle.dir = reflect(particle.dir, sampledNormal) * SLOWING;
    }
    
    if (g_time - particle. spawnTime > MAX_PARTICLE_LIFETIME) 
	{
		uint prev;
		InterlockedAdd(g_particlesRange[2], 1, prev);
	}
	
    g_particles[index] = particle;
	return;
}