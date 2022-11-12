#ifndef DISSOLUTION_PARTICLE_
#define DISSOLUTION_PARTICLE_

const static float MAX_PARTICLE_LIFETIME = 5.f;

struct Particle
{
    float3 position;
    float spawnTime;
    float3 dir;
    float alpha;
    float3 size;
    float3 color;
};

float particle_fading(float time, float lifetime)
{
    if (time < 0.5f)
        return 2.f * time;

    if (time > lifetime * 2.f / 3.f)
        return 3.f - 3.f * time / lifetime;

    return 1.f;
}


#endif