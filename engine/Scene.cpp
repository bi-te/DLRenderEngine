#include "Scene.h"

#include "moving/ObjectMover.h"
#include "moving/PointLightMover.h"
#include "moving/SphereMover.h"
#include "moving/SpotlightMover.h"
#include "moving/TransformMover.h"
#include "render/Lighting.h"
#include "render/light_render.h"

std::unique_ptr<ObjectMover> Scene::select_object(const Ray& ray, float t_min, float t_max, Intersection& record)
{
	record = Intersection::infinite();
	std::unique_ptr<ObjectMover> mover;

	for(PointLightObject& plobject: point_lights)
	{
		if(plobject.sphere.intersection(ray, t_min, t_max, record))
		{
			mover = std::make_unique<PointLightMover>(PointLightMover{plobject});
		}
	}

	for (SpotlightObject& slobject : spotlights)
	{
		if (slobject.sphere.intersection(ray, t_min, t_max, record))
		{
			mover = std::make_unique<SpotlightMover>(SpotlightMover{ slobject });
		}
	}

	for (SphereObject& object : spheres)
	{
		if (object.sphere.intersection(ray, t_min, t_max, record))
		{
			mover = std::make_unique<SphereMover>(SphereMover{ object.sphere });
		}
	}

	for (MeshInstance& mesh : meshes)
	{
		if (mesh.intersection(ray, t_min, t_max, record))
		{
			mover = std::make_unique<TransformMover>(TransformMover{ mesh.transform });
		}
	}

	return mover;
}

bool Scene::ray_collision(const Ray& ray, float t_min, float t_max, Intersection& nearest, uint& material_index) const
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

bool Scene::shadow_test(const Ray& ray, float t_min, float t_max) const
{
	Intersection record = Intersection::infinite();

	for (const SphereObject& object : spheres)
	{
		if( object.sphere.intersection(ray, t_min, t_max, record)) return true;
	}

	for (const MeshInstance& mesh : meshes)
	{
		if (mesh.intersection(ray, t_min, t_max, record)) return true;
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
	uint material;

	for (int i = 0; i < screen_width; ++i)
	{
		dx = float(i) / screen_width;
		for (int k = 0; k < screen_height; ++k)
		{
			dy = float(k) / screen_height;

			ray.direction = ((blpoint + right * dx + up * dy).head<3>() - ray.origin).normalized();

			if (ray_collision(ray, camera.zn, camera.zf, record,material))
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
				color = { 0, 15, 30 };
			}
			if (color.x() > 255) color.x() = 255;
			if (color.y() > 255) color.y() = 255;
			if (color.z() > 255) color.z() = 255;

			screen[k * screen_width + i] = {
				static_cast<unsigned char>(color.x()),
				static_cast<unsigned char>(color.y()),
				static_cast<unsigned char>(color.z()) };
		}


	}
}

void Scene::process_direct_light(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const
{
	Ray visible;
	visible.direction = -sunlight.direction;
	visible.origin = record.point + visible.direction * 0.01;

	if (shadow_test(visible, 0.f, std::numeric_limits<float>::infinity())) return;

	calc_direct_light(color, sunlight, record, camera_pos, m);
}

void Scene::process_point_lights(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const
{
	Ray visible;
	for (const PointLightObject& plobject : point_lights)
	{
		PointLight plight = plobject.plight;
		visible.direction = (plight.position - record.point).normalized();
		visible.origin = record.point + visible.direction * 0.01;

		float max_dist = (plight.position - record.point).norm() - 1.f;

		if (shadow_test(visible, 0.f, max_dist)) continue;

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
		visible.origin = record.point + visible.direction * 0.01;

		float max_dist = (spotlight.position - record.point).norm() - 1.f;

		if (shadow_test(visible, 0.f, max_dist)) continue;
		
		calc_spotlight(color, spotlight, record, camera_pos, m);
	}
}
