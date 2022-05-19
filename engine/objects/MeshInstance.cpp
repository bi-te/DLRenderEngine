#include "MeshInstance.h"

bool MeshInstance::intersection(const Ray& ray, float t_min, float t_max, Intersection& nearest,
	uint& material_index) const
{
	if(intersection(ray, t_min, t_max, nearest))
	{
		material_index = material;
		return true;
	}
	
	return false;
}

bool MeshInstance::intersection(const Ray& ray, float t_min, float t_max, Intersection& record) const
{
	float t;
	bool intersected = false;

	Ray transformed_ray;
	transformed_ray.origin = (vec4{ ray.origin.x(), ray.origin.y(), ray.origin.z(), 1.f } *transform.mtransform_inv).head<3>();
	transformed_ray.direction = ray.direction * transform.mtransform_inv.block<3, 3>(0, 0);

	vec3 a, b, c;

	vec3 ab, ac, ao;
	vec3 norm;

	for (int i = 0; i < mesh->vertices().size(); i += 9)
	{
		a = { mesh->vertices()[i], mesh->vertices()[i + 1], mesh->vertices()[i + 2] };
		b = { mesh->vertices()[i + 3], mesh->vertices()[i + 4], mesh->vertices()[i + 5] };
		c = { mesh->vertices()[i + 6], mesh->vertices()[i + 7], mesh->vertices()[i + 8] };

		ab = b - a;
		ac = c - a;
		ao = transformed_ray.origin - a;

		norm = ab.cross(ac);

		float det = -transformed_ray.direction.dot(norm);

		float u = -transformed_ray.direction.dot(ao.cross(ac)) / det;
		if (u < 0 || u > 1) continue;

		float v = -transformed_ray.direction.dot(ab.cross(ao)) / det;
		if (v < 0 || v + u > 1) continue;

		t = ao.dot(norm) / det;

		if (t > t_min && t < t_max && t < record.t)
		{
			intersected = true;
			record.t = t;
			record.point = ray.position(t);
			record.norm = (norm * transform.rotation().toRotationMatrix()).normalized();
		}
	}

	return intersected;
}
