#pragma once

#include "AppearingInstances.h"
#include "EmissiveInstances.h"
#include "OpaqueInstances.h"
#include "IntersectionQuery.h"

class MeshSystem
{
	static MeshSystem* s_system;
	MeshSystem() = default;

	MeshSystem(const MeshSystem& other) = delete;
	MeshSystem(MeshSystem&& other) noexcept = delete;
	MeshSystem& operator=(const MeshSystem& other) = delete;
	MeshSystem& operator=(MeshSystem&& other) noexcept = delete;

public:
	OpaqueInstances opaque_instances;
	EmissiveInstances emissive_instances;
	AppearingInstances appearing_instances;


	bool select_mesh(const Ray& ray, IntersectionQuery& record);

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

