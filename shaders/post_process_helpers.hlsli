#ifndef _POST_PROCESS_HELPERS_
#define _POST_PROCESS_HELPERS_

float3 adjust_exposure(float3 color, float ev100)
{
	float lmax = 1.2f * pow(2.f, ev100);
	return color * (1.f / lmax);
}

float3 aces_tonemap(float3 color)
{
	row_major float3x3 m1 = float3x3(
		0.59719f, 0.07600f, 0.02840f,
		0.34548f, 0.90834f, 0.13383f,
		0.04823f, 0.01566f, 0.83777f
		);

	row_major float3x3 m2 = float3x3(
		1.60475f, -0.10208f, -0.00327f,
		-0.53108f, 1.10813f, -0.07276f,
		-0.07367f, -0.00605f, 1.07602f
		);

	float3 v = mul(color, m1);
	float3 a = v * (v + 0.0245786f) - 0.000090537f;
	float3 b = v * (0.983729f * v + 0.4329510f) + 0.238081f;
	float3 temp = mul(a / b, m2);

	return clamp(temp, 0.f, 1.f);
}

float3 gamma_correction(float3 color)
{
	return pow(color, 1.f / 2.2f);
}

#endif