#pragma once
#include "data_structures/solid_vector.h"
#include "math/Transform.h"

class TransformSystem
{
	static TransformSystem* s_system;
	TransformSystem() = default;

	TransformSystem(const TransformSystem& other) = delete;
	TransformSystem(TransformSystem&& other) noexcept = delete;
	TransformSystem& operator=(const TransformSystem& other) = delete;
	TransformSystem& operator=(TransformSystem&& other) noexcept = delete;
public:

	solid_vector<Transform> transforms;

	static void init()
	{
		if (s_system) reset();

		s_system = new TransformSystem;
	}

	static TransformSystem& instance()
	{
		assert(s_system && "TransformSystem not initialized");
		return *s_system;
	}

	static void reset()
	{
		delete s_system;
	}
};

