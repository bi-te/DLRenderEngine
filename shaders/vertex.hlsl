struct vs_in
{
	float3 pos: POSITION;
	float3 color: COLOR;
};

struct vs_out
{
	float4 color: COLOR;
	float4 position: SV_POSITION;
};

vs_out main(vs_in input)
{
	vs_out res;
	res.color = float4(input.color, 1.f);
	res.position = float4(input.pos, 1.f);
	return res;
}