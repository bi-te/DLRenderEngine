#ifndef _FULLSCREEN_
#define _FULLSCREEN_

#include "globals.hlsli"

struct vs_out {
	float4 pos: SV_Position;
	float3 texcoord: TexCoord;
};

vs_out fullscreenVertex(uint index)
{
	vs_out res;
	res.pos = float4(-1.f + 4.f * (index == 2), -1.f + 4.f * (index == 1), 0.f, 1.f);
	res.texcoord = g_frustum.bottom_left.xyz
		+ 2.f * (index == 2) * g_frustum.right_vector.xyz
		+ 2.f * (index == 1) * g_frustum.up_vector.xyz;

	return res;
}

#endif