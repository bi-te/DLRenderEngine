
RWBuffer<uint> g_particlesRange : register(u0); //[0] - begin, [1] - count, [2] - deathcount;
RWBuffer<uint> g_particlesInitialArgs : register(u2); //[0] - vertex count, [1] - instance count, [2/3] - vertex/instance start loc(0)

cbuffer ParticlesSizeBuffer : register(b1)
{
    uint g_particlesBufferSize;
}

[numthreads(1, 1, 1)]
void cs_main( uint3 DTid : SV_DispatchThreadID )
{
    g_particlesRange[0] = (g_particlesRange[0] + g_particlesRange[2]) % g_particlesBufferSize;
    g_particlesRange[1] = g_particlesRange[1] - g_particlesRange[2];
    g_particlesRange[2] = 0;
    
    g_particlesInitialArgs[0] = 6u;
    g_particlesInitialArgs[1] = clamp(g_particlesRange[1], 0, g_particlesBufferSize);
}