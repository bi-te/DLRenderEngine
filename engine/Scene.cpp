#include "Scene.h"

#include <iostream>

#include "moving/SphereMover.h"
#include "moving/PointLightMover.h"
#include "moving/SpotlightMover.h"
#include "moving/TransformMover.h"
#include "render/light_render.h"
#include "RandomGenerator.h"

void Scene::select_object(const Ray& ray, float t_min, float t_max, IntersectionQuery& record)
{
	objectRef ref;
	record.intersection = Intersection::infinite();

	for(PointLightObject& plobject: point_lights)
		if (plobject.sphere.intersection(ray, t_min, t_max, record.intersection)){
			ref.type = POINTLIGHT;
			ref.ptr = &plobject;
		}

	for (SpotlightObject& slobject : spotlights)
		if (slobject.sphere.intersection(ray, t_min, t_max, record.intersection)){
			ref.type = SPOTLIGHT;
			ref.ptr = &slobject;
		}

	for (SphereObject& object : spheres)
		if (object.sphere.intersection(ray, t_min, t_max, record.intersection)){
			ref.type = SPHERE;
			ref.ptr = &object;
		}

	for (MeshInstance& mesh : meshes)
		if (mesh.intersection(ray, t_min, t_max, record.intersection)){
			ref.type = MESH;
			ref.ptr = &mesh;
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

bool Scene::ray_light_collision(const Ray& ray, float t_min, float t_max, Intersection& nearest,
	uint32_t& material_index) const
{
	bool hit = false;
	for (const PointLightObject& plobject : point_lights)
		hit |= plobject.intersection(ray, t_min, t_max, nearest, material_index);

	for (const SpotlightObject& slobject : spotlights)
		hit |= slobject.intersection(ray, t_min, t_max, nearest, material_index);

	return hit;
}

bool Scene::ray_object_collision(const Ray& ray, float t_min, float t_max, Intersection& nearest,
	uint32_t& material_index) const
{
	bool hit = false;
	for (const SphereObject& object : spheres)
		hit |= object.intersection(ray, t_min, t_max, nearest, material_index);

	for (const MeshInstance& mesh : meshes)
		hit |= mesh.intersection(ray, t_min, t_max, nearest, material_index);

	hit |= floor.intersection(ray, t_min, t_max, nearest, material_index);
	
	return hit;
}

bool Scene::shadow_test(const Ray& ray, float t_max) const
{
	Intersection record = Intersection::infinite();
	for (const MeshInstance& mesh : meshes)
		if (mesh.intersection(ray, 0.f, t_max, record)) return true;

	for (const SphereObject& object : spheres)
		if (object.sphere.intersection(ray, 0.f, t_max, record)) return true;

	if (floor.plane.intersection(ray, 0.f, t_max, record))return true;

	return false;
}

vec3 Scene::reflection(Ray& ray, uint8_t depth, uint8_t max_depth, float t_max) const
{
	Intersection record;
	uint32_t material;
	vec3 color;

	record = Intersection::infinite();
	if (!ray_object_collision(ray, 0.f, t_max, record, material))
		return AMBIENT;

	const Material& m = materials.at(material);
	color = m.emission;

	color += AMBIENT.cwiseProduct(m.albedo);
	process_direct_light(color, record, ray.origin, m);
	process_point_lights(color, record, ray.origin, m);
	process_spotlights(color, record, ray.origin, m);

	if (m.roughness < MAX_REFLECTIVE_ROUGHNESS && depth < max_depth)
	{
		Ray refl;
		refl.direction = reflect(ray.direction, record.norm);
		refl.origin = record.point + refl.direction * 0.001f;
		vec3 reflcolor = reflection(refl, depth + 1, max_depth, t_max) * (MAX_REFLECTIVE_ROUGHNESS - m.roughness) * 10;
		
		cook_torrance(
			color, refl.direction, record.norm, -ray.direction,
			reflcolor, 1.f, m);
	}

	return color;
}

void Scene::draw(Screen& screen, ImageSettings& image, const Camera& camera) const
{
	if (image.global_illumination == GI_COMPLETED) return;

	for (uint16_t row = 0; row < screen.buffer_height(); ++row)
		for (uint16_t column = 0; column < screen.buffer_width(); ++column)
			draw_pixel(screen, image, camera, row, column);

	if (image.global_illumination == GI_ON) image.global_illumination = GI_COMPLETED;
}

void Scene::draw(Screen& screen, ImageSettings& image, const Camera& camera, ParallelExecutor& executor) const
{
	if (image.global_illumination == GI_COMPLETED) return;

	static auto fdraw = [this, &screen, &image, &camera](uint32_t threadIndex, uint32_t taskIndex)
	{
		draw_pixel(screen, image, camera, taskIndex / screen.buffer_width(), taskIndex % screen.buffer_width());
	};
	executor.execute(fdraw, screen.buffer_width() * screen.buffer_height(), 50);

	if (image.global_illumination == GI_ON) image.global_illumination = GI_COMPLETED;
}

void Scene::draw_pixel(Screen& screen, ImageSettings& image, const Camera& camera, uint32_t row, uint32_t column) const
{
	Intersection record = Intersection::infinite();
	Ray ray;
	ray.origin = camera.position();

	float dy = (row + 0.5f) / screen.buffer_height();
	float dx = (column + 0.5f) / screen.buffer_width();
	ray.direction = 
		((camera.blnear_fpoint + camera.frustrum_right * dx + camera.frustrum_up * dy).head<3>() - ray.origin).normalized();

	vec3 color;
	uint32_t material;
	ray_object_collision(ray, camera.zn, camera.zf, record, material);
	ray_light_collision(ray, camera.zn, camera.zf, record, material);
	if (std::isfinite(record.t))
	{
		const Material& m = materials.at(material);
		color = m.emission;

		if (m.type == SURFACE)
		{
			process_direct_light(color, record, ray.origin, m);
			process_point_lights(color, record, ray.origin, m);
			process_spotlights(color, record, ray.origin, m);

			if (image.global_illumination == GI_ON)
			{
				light_integral(color, ray.origin, m, record, image.gi_tests);
			}
			else if (image.progressive_gi)
			{
				Ray integral_ray;
				vec3 random, refl  = reflect(ray.direction, record.norm);
				vec3 light, view = (ray.origin - record.point).normalized();

				mat3 basis = mat3::Identity();
				basis.row(1) = record.norm;
				onb_frisvad(basis);
				random = vec3{
					   2.f * RandomGenerator::generator().get_real() - 1.f,
					   RandomGenerator::generator().get_real(),
					   2.f * RandomGenerator::generator().get_real() - 1.f
				}.normalized() * basis;				

				integral_ray.direction = lerp(refl, random, m.roughness).normalized();
				integral_ray.origin = record.point + integral_ray.direction * 0.001f;

				integral_test(integral_ray, MAX_PROCESS_DISTANCE, light);
				cook_torrance(color, integral_ray.direction, record.norm,
					view, light, 0.5f, m);

				integral_ray.direction = random.normalized();
				integral_ray.origin = record.point + integral_ray.direction * 0.001f;

				integral_test(integral_ray, MAX_PROCESS_DISTANCE, light);
				cook_torrance(color, integral_ray.direction, record.norm, 
					view, light, 0.5f , m);
			}
			else
			{
				color += AMBIENT.cwiseProduct(m.albedo);
				if (image.reflection && m.roughness < MAX_REFLECTIVE_ROUGHNESS)
				{
					Ray refl;
					refl.direction = reflect(ray.direction, record.norm);
					refl.origin = record.point + refl.direction * 0.01f;
					vec3 reflcolor = lerp(
						vec3(0, 0, 0),  
						reflection(refl, 1, MAX_REFLECTION_DEPTH, MAX_PROCESS_DISTANCE),
						(MAX_REFLECTIVE_ROUGHNESS - m.roughness) * 10
					);
										
					cook_torrance(color, refl.direction, record.norm, -ray.direction, 
					              reflcolor, 1.f, m);
				}
			}	
		}
	}
	else
	{
		color = AMBIENT;
	}

	adjust_exposure(color, image.ev100);
	aces_tonemap(color);
	gamma_correction(color);
	color *= 255;

	if (image.progressive_gi)
	{
		float weight = 1.f / min(image.gi_frame + 1.f, image.gi_tests);

		color = lerp(screen.fbuffer_.at(row * screen.buffer_width() + column), color, weight);
		screen.fbuffer_.at(row * screen.buffer_width() + column) = color;
	}
	screen.set(row, column, color);
}

void Scene::integral_test(const Ray& integral_ray, float t_max, vec3& light) const
{
	light = AMBIENT;
	uint32_t m;

	Intersection record = Intersection::infinite();

	if (!ray_object_collision(integral_ray, 0.f, t_max, record, m))
		return;

	const Material& material = materials.at(m);
	light = AMBIENT.cwiseProduct(material.albedo);
	light += material.emission;
	process_point_lights(light, record, integral_ray.origin, material);
	process_spotlights(light, record, integral_ray.origin, material);
	process_direct_light(light, record, integral_ray.origin, material);

}

void Scene::light_integral(vec3& color, const vec3& camera_pos, const Material& material, 
	const Intersection& record, uint32_t tests) const
{
	RandomGenerator& rndm = RandomGenerator::generator();
	mat3 basis = mat3::Identity();
	basis.row(1) = record.norm;
	onb_frisvad(basis);
	std::vector set(fibonacci_set(tests, 2 * rndm.get_real() * PI));

	Ray integral_ray;
	vec3 light, view = (camera_pos - record.point).normalized();
	float dw = 1.f / (tests + 1.f);
	for (const vec3& point : set)
	{
		integral_ray.direction = (point * basis).normalized();
		integral_ray.origin = record.point + integral_ray.direction * 0.001f;
		integral_test(integral_ray, MAX_PROCESS_DISTANCE, light);
		cook_torrance(color, integral_ray.direction, record.norm, view, light, dw, material);
	}

	integral_ray.direction = reflect(-view, record.norm);
	integral_ray.origin = record.point + integral_ray.direction * 0.001f;
	integral_test(integral_ray, MAX_PROCESS_DISTANCE, light);
	cook_torrance(color, integral_ray.direction, record.norm, view, light, dw, material);
}

void Scene::process_direct_light(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const
{
	Ray light;
	light.direction = -dirlight.direction;
	light.origin = record.point + light.direction * 0.01f;
	vec3 view = (camera_pos - record.point).normalized();

	if (shadow_test(light, std::numeric_limits<float>::infinity())) return;
	cook_torrance_aprox(color, light.direction, light.direction, record.norm, view,
	                    dirlight.light, 0.5 * dirlight.solid_angle / PI, m);
}

void Scene::process_point_lights(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const
{
	Ray light;
	vec3 view = (camera_pos - record.point).normalized();
	for (const PointLightObject& plobject : point_lights)
	{
		light.direction = (plobject.plight.position - record.point).normalized();
		light.origin = record.point + light.direction * 0.01f;
		float max_dist = (plobject.plight.position - record.point).norm();

		if (shadow_test(light, max_dist)) continue;
		calc_point_light_pbr(color, light.direction, plobject.plight.light, 
		                     max_dist, plobject.sphere.radius, record.norm, view, m);
	}
}

void Scene::process_spotlights(vec3& color, const Intersection& record, const vec3& camera_pos, const Material& m) const
{
	Ray visible;
	vec3 view = (camera_pos - record.point).normalized();
	for (const SpotlightObject& slobject : spotlights)
	{
		Spotlight spotlight = slobject.spotlight;
		visible.direction = (spotlight.position - record.point).normalized();
		visible.origin = record.point + visible.direction * 0.01f;
		float max_dist = (spotlight.position - record.point).norm() - slobject.sphere.radius;

		if (shadow_test(visible, max_dist)) continue;		
		calc_spotlight_pbr(color, spotlight, slobject.sphere.radius, record, view, m);
	}
}
