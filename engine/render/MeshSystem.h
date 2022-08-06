#pragma once

#include "OpaqueInstances.h"

class MeshSystem
{
	static MeshSystem* s_system;
	MeshSystem(){}
public:
	OpaqueInstances opaque_instances;


	void render()
	{
		opaque_instances.render();
	}

	static void init()
	{
		if (s_system) reset();

		s_system = new MeshSystem;
	}

	static MeshSystem& instance()
	{
		assert(s_system && "TextureManager not initialized");
		return *s_system;
	}

	static void reset()
	{
		delete s_system;
	}
};

