#include "Scene.h"

#include "moving/SphereMover.h"
#include "moving/PointLightMover.h"
#include "moving/SpotlightMover.h"
#include "moving/TransformMover.h"
#include "objects/MeshInstance.h"
#include "render/Lighting.h"
#include "render/light_render.h"

void Scene::select_object(const Ray& ray, float t_min, float t_max, IntersectionQuery& record)
{
	record.intersection = Intersection::infinite();
	objectRef ref;

	for(PointLightObject& plobject: point_lights)
	{
		if(plobject.sphere.intersection(ray, t_min, t_max, record.intersection))
		{
			ref.type = POINTLIGHT;
			ref.ptr = &plobject;
		}
	}

	for (SpotlightObject& slobject : spotlights)
	{
		if (slobject.sphere.intersection(ray, t_min, t_max, record.intersection))
		{
			ref.type = SPOTLIGHT;
			ref.ptr = &slobject;
		}
	}

	for (SphereObject& object : spheres)
	{
		if (object.sphere.intersection(ray, t_min, t_max, record.intersection))
		{
			ref.type = SPHERE;
			ref.ptr = &object;
		}
	}

	for (MeshInstance& mesh : meshes)
	{
		if (mesh.intersection(ray, t_min, t_max, record.intersection))
		{
			ref.type = MESH;
			ref.ptr = &mesh;
		}
	}

	switch (ref.type)
	{
	case SPHERE: record.mover = std::make_unique<SphereMover>(static_cast<SphereObject*>(ref.ptr)->sphere); break;
	case MESH: record.mover = std::make_unique<TransformMover>(static_cast<MeshInstance*>(ref.ptr)->transform); break;
	case POINTLIGHT: record.mover = std::make_unique<PointLightMover>(*static_cast<PointLightObject*>(ref.ptr)); break;
	case SPOTLIGHT: record.mover = std::make_unique<SpotlightMover>(*static_cast<SpotlightObject*>(ref.ptr)); break;
	case NONE: record.mover = nullptr;
	}
}

bool Scene::ray_collision(const Ray& ray, float t_min, float t_max, Intersection& nearest, uint32_t& material_index) const
{
	Intersection record = Intersection::infinite();
	bool hit = false;

	for (const PointLightObject& plobject : point_lights)
	{
		hit |= plobject.intersection(ray, t_min, t_max, record, material_index);
	}

	for (const SpotlightObject& slobject : spotlights)
	{
		hit |= slobject.intersection(ray, t_min, t_max, record, material_index);
	}

	for (const SphereObject& object : spheres)
	{
		hit |= object.intersection(ray, t_min, t_max, record, material_index);
	}

	for (const MeshInstance & mesh : meshes)
	{
		hit |= mesh.intersection(ray, t_min, t_max, record, material_index);
	}

	hit |= floor.intersection(ray, t_min, t_max, record, material_index);;

	nearest = record;

	return hit;
}

bool Scene::shadow_test(const Ray& ray, float t_max) const
{
	Intersection record = Intersection::infinite();

	for (const SphereObject& object : spheres)
	{
		if( object.sphere.intersection(ray, 0.f, t_max, record)) return true;
	}

	for (const MeshInstance& mesh : meshes)
	{
		if (mesh.intersection(ray, 0.f, t_max, record)) return true;
	}

	return false;
}

void Scene::draw(Screen& screen, const Camera& camera)
{
	uint16_t screen_width = screen.buffer_width();
	uint16_t screen_height = screen.buffer_height();
	Intersection record;

	vec4 blpoint = camera.blnear_fpoint;
	vec4 tlpoint = camera.tlnear_fpoint;
	vec4 brpoint = camera.brnear_fpoint;

	vec4 up = tlpoint - blpoint;
	vec4 right = brpoint - blpoint;

	Ray ray;
	ray.origin = camera.position();

	float dx, dy;
	vec3 color;
	uint32_t material;

	for (uint16_t row = 0; row < screen_height; ++row)
	{
		dy = (row + 0.5f) / screen_height;
		for (uint16_t column = 0; column < screen_width; ++column)
		{
			dx = (column + 0.5f) / screen_width;

			ray.direction = ((blpoint + right * dx + up * dy).head<3>() - ray.origin).normalized();

			if (ray_collision(ray, camera.zn, camera.zf, record, material))
			{
				Material& m = materials.at(material);
				color = m.emission;

				if (m.type == SURFACE)
				{
					color += AMBIENT.cwiseProduct(m.albedo);

					process_direct_light(color, record, ray.origin, m);
					process_point_lights(color, record, ray.origin, m);
					process_spotlights(color, record, ray.origin, m);
				}
			}
			else
			{
				color = AMBIENT;
			}
			screen.set(row, column, color);
		}
	}
}

void Scene::process_direct_light(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const
{
	Ray visible;
	visible.direction = -sunlight.direction;
	visible.origin = record.point + visible.direction * 0.01f;

	if (shadow_test(visible, std::numeric_limits<float>::infinity())) return;

	calc_direct_light(color, sunlight, record, camera_pos, m);
}

void Scene::process_point_lights(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const
{
	Ray visible;
	for (const PointLightObject& plobject : point_lights)
	{
		PointLight plight = plobject.plight;
		visible.direction = (plight.position - record.point).normalized();
		visible.origin = record.point + visible.direction * 0.01f;

		float max_dist = (plight.position - record.point).norm() - 1.f;

		if (shadow_test(visible, max_dist)) continue;

		calc_point_light(color, plight, record, camera_pos, m);
	}
}

void Scene::process_spotlights(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const
{
	Ray visible;
	for (const SpotlightObject& slobject : spotlights)
	{
		Spotlight spotlight = slobject.spotlight;
		visible.direction = (spotlight.position - record.point).normalized();
		visible.origin = record.point + visible.direction * 0.01f;

		float max_dist = (spotlight.position - record.point).norm() - 1.f;

		if (shadow_test(visible, max_dist)) continue;
		
		calc_spotlight(color, spotlight, record, camera_pos, m);
	}
}
