cbuffer view_projection_matrix: register(b0)
{
	matrix <float, 4, 4> g_viewProj;
}

SamplerState g_sampler: register(s0);