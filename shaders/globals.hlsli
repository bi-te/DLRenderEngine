struct Frustum
{
	float4 bottom_left;
	float4 up_vector;
	float4 right_vector;
};

cbuffer perFrame: register(b0)
{
	matrix <float, 4, 4> g_viewProj;
	Frustum frustum;
}

SamplerState g_sampler: register(s0);