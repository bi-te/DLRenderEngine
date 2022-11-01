#include "MeshSystem.h"

#include "ModelManager.h"
#include "moving/TransformSystem.h"

MeshSystem* MeshSystem::s_system;

bool MeshSystem::mesh_intersection(const Ray& ray, IntersectionQuery& record, const Model& model, ID transformId)
{
	TransformSystem& transform_system = TransformSystem::instance();
	Ray transformed_ray;
	Ray tray;

	Transform& to_model = transform_system.transforms[transformId];
	transformed_ray.origin = (vec4f{ ray.origin.x(), ray.origin.y(), ray.origin.z(), 1.f } *to_model.matrix_inv()).head<3>();
	transformed_ray.direction = (ray.direction * to_model.matrix_inv().topLeftCorner<3, 3>());
	for (uint32_t i = 0; i < model.octrees.size(); i++)
	{
		const TriangleOctree& octree = model.octrees[i];
		for (uint32_t mesh_mat_ind : model.meshes[i].mesh_matrices)
		{
			const mat4f& mesh_inv = model.tree[mesh_mat_ind].mesh_matrix_inv;
			const mat4f& mesh_mat = model.tree[mesh_mat_ind].mesh_matrix;

			tray.origin = (vec4f{ transformed_ray.origin.x(), transformed_ray.origin.y(), transformed_ray.origin.z(), 1.f } *mesh_inv).head<3>();
			tray.direction = (transformed_ray.direction * mesh_inv.topLeftCorner<3, 3>());

			if (octree.intersect(tray, record.intersection))
			{
				record.transformId = transformId;
				record.intersection.point = ray.position(record.intersection.t);
				record.intersection.norm = record.intersection.norm * mesh_mat.topLeftCorner<3, 3>() * to_model.normal_matrix;
				record.intersection.norm.normalize();
				return true;
			}
		}
	}

	return false;
}

bool MeshSystem::select_mesh(const Ray& ray, IntersectionQuery& record)
{
	bool intersected = false;

	for (const auto& opaque_model : opaque_instances.perModels)
		for (const auto& instance : opaque_model.instances)
			intersected |= mesh_intersection(ray, record, *opaque_model.model.get(), instance.model_world);

	for (const auto& emissive_model : emissive_instances.perModels)
		for (const auto& instance : emissive_model.instances)
			intersected |= mesh_intersection(ray, record, *emissive_model.model.get(), instance.model_world);

	for (const auto& appearing_model : appearing_instances.perModels)
		for (const auto& instance : appearing_model.instances)
			intersected |= mesh_intersection(ray, record, *appearing_model.model.get(), instance.model_world);

	return intersected;
}
