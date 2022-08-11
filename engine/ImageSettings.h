#pragma once
#include <cstdint>

enum GLOBAL_ILLUMINATION { GI_OFF, GI_ON, GI_COMPLETED };
struct ImageSettings
{
	float ev100;
	bool reflection;
	bool progressive_gi;
	GLOBAL_ILLUMINATION global_illumination;
	uint32_t gi_tests, gi_frame;
	std::vector<vec3f> hemisphere_points;

	void set_gi_tests(uint32_t tests)
	{
		gi_tests = tests;
		hemisphere_points.resize(tests);
	}
};
