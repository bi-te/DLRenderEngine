#include "MeshSystem.h"

#include "ModelManager.h"
#include "moving/TransformSystem.h"

MeshSystem* MeshSystem::s_system;

bool MeshSystem::select_mesh(const Ray& ray, Intersection& nearest)
{
	TransformSystem& transforms = TransformSystem::instance();

	bool intersected = false;

	Ray transformed_ray;
	Ray tray;
	for (const auto& opaque_model : opaque_instances.perModels)
	{
		Model& model = *opaque_model.model.get();
		for (const auto& instance : opaque_model.instances)
		{
			Transform&  to_model = transforms.transforms[instance.model_world];
			transformed_ray.origin = (vec4f{ ray.origin.x(), ray.origin.y(), ray.origin.z(), 1.f } * to_model.matrix_inv()).head<3>();
			transformed_ray.direction = (ray.direction * to_model.matrix_inv().topLeftCorner<3, 3>());

			for (uint32_t i = 0; i < model.octrees.size(); i++)
			{
				TriangleOctree& octree = model.octrees[i];

				for(uint32_t mesh_mat_ind: model.meshes[i].mesh_matrices)
				{
					mat4f& mesh_inv = model.tree[mesh_mat_ind].mesh_matrix_inv;

					tray.origin = (vec4f{ transformed_ray.origin.x(), transformed_ray.origin.y(), transformed_ray.origin.z(), 1.f } *mesh_inv).head<3>();
					tray.direction = (transformed_ray.direction * mesh_inv.topLeftCorner<3, 3>());

					if (octree.intersect(tray, nearest))
					{
						nearest.transormId = instance.model_world;
						nearest.point = ray.position(nearest.t);
						nearest.norm = nearest.norm *  mesh_inv.topLeftCorner<3, 3>() * to_model.normal_matrix;
						intersected = true;
					}
				}
			}
		}
	}

	for (const auto& opaque_model : emissive_instances.perModels)
	{
		Model& model = *opaque_model.model.get();
		for (const auto& instance : opaque_model.instances)
		{
			Transform& to_model = transforms.transforms[instance.model_world];
			transformed_ray.origin = (vec4f{ ray.origin.x(), ray.origin.y(), ray.origin.z(), 1.f } *to_model.matrix_inv()).head<3>();
			transformed_ray.direction = (ray.direction * to_model.matrix_inv().topLeftCorner<3, 3>());

			for (uint32_t i = 0; i < model.octrees.size(); i++)
			{
				TriangleOctree& octree = model.octrees[i];

				for (uint32_t mesh_mat_ind : model.meshes[i].mesh_matrices)
				{
					mat4f& mesh_inv = model.tree[mesh_mat_ind].mesh_matrix_inv;

					tray.origin = (vec4f{ transformed_ray.origin.x(), transformed_ray.origin.y(), transformed_ray.origin.z(), 1.f } *mesh_inv).head<3>();
					tray.direction = (transformed_ray.direction * mesh_inv.topLeftCorner<3, 3>());

					if (octree.intersect(tray, nearest))
					{
						nearest.transormId = instance.model_world;
						nearest.point = ray.position(nearest.t);
						nearest.norm = nearest.norm * mesh_inv.topLeftCorner<3, 3>() * to_model.normal_matrix;
						intersected = true;
					}
				}
			}
		}
	}

	return intersected;
}
