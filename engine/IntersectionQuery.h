#pragma once
#include "math/Intersection.h"
#include "moving/ObjectMover.h"
#include "moving/PointLightMover.h"
#include "moving/SphereMover.h"
#include "moving/SpotlightMover.h"
#include "moving/TransformMover.h"
#include "objects/MeshInstance.h"

enum ObjType { SPHERE, MESH, POINTLIGHT, SPOTLIGHT, NONE};

struct objectRef
{
	void* ptr;
	ObjType type = NONE;
};

struct IntersectionQuery
{
	Intersection intersection;
	objectRef object;
	std::unique_ptr<ObjectMover> mover;

	void update_mover()
	{
		switch (object.type)
		{
		case SPHERE: mover = std::make_unique<SphereMover>(static_cast<SphereObject*>(object.ptr)->sphere); break;
		case MESH: mover = std::make_unique<TransformMover>(static_cast<MeshInstance*>(object.ptr)->transform); break;
		case POINTLIGHT: mover = std::make_unique<PointLightMover>(*static_cast<PointLightObject*>(object.ptr)); break;
		case SPOTLIGHT: mover = std::make_unique<SpotlightMover>(*static_cast<SpotlightObject*>(object.ptr)); break;
		case NONE: mover = nullptr;
		}
	}
	
};