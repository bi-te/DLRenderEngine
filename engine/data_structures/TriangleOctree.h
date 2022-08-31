#pragma once

#include "math/math.h"
#include "math/AABB.h"
#include "math/Ray.h"

struct Mesh;

class TriangleOctree
{
public:
	const static int PREFFERED_TRIANGLE_COUNT;
	const static float MAX_STRETCHING_RATIO;

	void clear() { m_mesh = nullptr; }
	bool inited() const { return m_mesh != nullptr; }

	void initialize(const Mesh& mesh);

	bool intersect(const Ray& ray, Intersection& nearest) const;
protected:

	const Mesh* m_mesh = nullptr;
	
	std::vector<uint32_t> m_triangles;

	BoundingBox m_box;
	BoundingBox m_initialBox;

	std::unique_ptr<std::array<TriangleOctree, 8>> m_children;

	void initialize(const Mesh& mesh, const BoundingBox& parentBox, const vec3f& parentCenter, int octetIndex);

	bool addTriangle(uint32_t triangleIndex, const vec3f& V1, const vec3f& V2, const vec3f& V3, const vec3f& center);

	bool intersectInternal(const Ray& ray, Intersection& nearest) const;
};


