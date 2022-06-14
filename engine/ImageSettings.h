#pragma once
#include <cstdint>

enum GLOBAL_ILLUMINATION { GI_OFF, GI_ON, GI_COMPLETED };
struct ImageSettings
{
	float ev100;
	float max_reflect_distance;
	bool reflection;
	GLOBAL_ILLUMINATION global_illumination;
	uint32_t gi_tests;
	uint8_t max_reflect_depth;
};
