#pragma once

#include "math/math.h"

struct Skybox
{
	std::wstring texture, skyshader;

	void render();
};

