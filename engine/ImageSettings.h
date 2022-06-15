#pragma once
#include <cstdint>

enum GLOBAL_ILLUMINATION { GI_OFF, GI_ON, GI_COMPLETED };
struct ImageSettings
{
	float ev100;
	bool reflection;
	GLOBAL_ILLUMINATION global_illumination;
	uint32_t gi_tests;
};
