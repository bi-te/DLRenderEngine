#pragma once

#include "EmissiveInstances.h"
#include "OpaqueInstances.h"
#include "math/Ray.h"

class MeshSystem
{
	static MeshSystem* s_system;
	MeshSystem() = default;
public:
	OpaqueInstances opaque_instances;
	EmissiveInstances emissive_instances;


	bool select_mesh(const Ray& ray, Intersection& nearest);

	void render()
	{
		opaque_instances.render();
		emissive_instances.render();
	}

	static void init()
	{
		if (s_system) reset();

		s_system = new MeshSystem;
	}

	static MeshSystem& instance()
	{
		assert(s_system && "MeshSystem not initialized");
		return *s_system;
	}

	static void reset()
	{
		delete s_system;
	}
};

