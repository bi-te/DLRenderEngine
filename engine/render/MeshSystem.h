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

	bool mesh_intersection(const Ray& ray, IntersectionQuery& record, const Model& model, ID transformId);

public:
	OpaqueInstances opaque_instances;
	EmissiveInstances emissive_instances;
	AppearingInstances appearing_instances;

	bool select_mesh(const Ray& ray, IntersectionQuery& record);

	void render(bool forward_rendering)
	{
		opaque_instances.render(forward_rendering);
		emissive_instances.render(forward_rendering);
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

