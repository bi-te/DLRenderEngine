#include "globals.hlsli"
#include "dissolution_particle.hlsli"

Buffer<uint> g_particlesRange : register(t0); //[0] - begin, [1] - count, [2] - deathcount;
StructuredBuffer<Particle> g_particles : register(t1);

struct vs_out
{
    float4 position : Sv_Position;
    nointerpolation float alpha : Alpha;
    nointerpolation float3 color : COLOR;
};

cbuffer ParticlesSizeBuffer : register(b1)
{
    uint g_particlesBufferSize;
}

vs_out main(uint index : Sv_VertexID, uint instanceIndex : SV_InstanceID)
{
    uint id = (instanceIndex + g_particlesRange[0]) % g_particlesBufferSize;
    Particle input = g_particles[id];
    
    float3 x = X_VEC * input.size.x;
    float3 y = Y_VEC * input.size.y;
    
    float3 camera_right = normalize(g_frustum.right_vector).xyz;
    float3 camera_up = normalize(g_frustum.up_vector).xyz;
    float3 camera_front = normalize(cross(camera_right, camera_up));

    vs_out res;
    float3x3  camera_rotation = float3x3(
		camera_right,
		camera_up,
		camera_front
	);
	
    res.position = float4(input.position, 1.f);
    
    float3 add_point = 0.f;
    switch (index)
    {
        case 0:
        case 3:
            add_point += -x - y;
            break;
        case 1:
            add_point += -x + y;
            break;
        case 2:
        case 4:
            add_point += +x + y;
            break;
        case 5:
            add_point += +x - y;
            break;
    }
    res.position.xyz += mul(add_point, camera_rotation);
    
    res.position = mul(g_viewProj, res.position);
    res.alpha = input.alpha;
    res.color = input.color;
    return res;
}

float4 ps_main(vs_out input) : Sv_Target
{    
    return float4(input.color, input.alpha);;
}