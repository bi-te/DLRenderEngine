#pragma once
#include <cstdint>

enum GLOBAL_ILLUMINATION { GI_OFF, GI_ON, GI_COMPLETED };
struct ImageSettings
{
	float ev100;
	bool reflection;
	bool progressive_gi;
	GLOBAL_ILLUMINATION global_illumination;
	uint32_t gi_frame;
	std::vector<vec3> hemisphere_points;

	void set_gi_tests(uint32_t tests)
	{
		hemisphere_points.resize(tests);
	}
};
