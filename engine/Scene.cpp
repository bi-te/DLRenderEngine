#include "Scene.h"

bool Scene::ray_collision(const Ray& ray, float t_min, float t_max, Intersection& record) const
{
	Intersection nearest = Intersection::infinite();
	bool hit = false;

	for (const SphereObject& object : spheres)
	{
		hit |= object.intersection(ray, t_min, t_max, nearest);
	}
	record = nearest;

	return hit;
}

void Scene::draw(Screen& screen, const Camera& camera)
{
	uint16_t screen_width = screen.width();
	uint16_t screen_height = screen.height();
	Intersection record;

	vec4 blpoint = vec4{ -1, -1, 1, 1} * camera.proj_inv;
	blpoint /= blpoint.w();

	vec4 tlpoint = vec4{ -1, 1, 1, 1 } * camera.proj_inv;
	tlpoint /= tlpoint.w();

	vec4 brpoint = vec4{ 1, -1, 1, 1 } * camera.proj_inv;
	brpoint /= brpoint.w();

	vec4 up = tlpoint - blpoint;
	vec4 right = brpoint - blpoint;

	float dx, dy;

	Ray ray;
	ray.origin = {0, 0, 0};

	for (int i = 0; i < screen_width; ++i)
	{
		dx = float(i) / screen_width;
		vec4 ddir = blpoint + right * dx;
		for (int k = 0; k < screen_height; ++k)
		{
			dy = float(k) / screen_height;
			
			vec4 dir = ddir + up * dy;
			dir.w() = 0;
			ray.direction = dir;
			ray.direction = normalize(ray.direction);

			if(i == 400 && k == 300)
			{
				dir *= 1;
			}

			if (ray_collision(ray, 0, std::numeric_limits<float>::infinity(), record))
			{
				Vec3 color = materials.at(record.material).emission + AMBIENT * materials.at(record.material).albedo;

				calc_direct_light(color, record);
				calc_point_lights(color, record);
				calc_spotlights(color, record);

				if (color.x > 255) color.x = 255;
				if (color.y > 255) color.y = 255;
				if (color.z > 255) color.z = 255;

				screen[k * screen_width + i] = {
					static_cast<unsigned char>(color.x),
					static_cast<unsigned char>(color.y),
					static_cast<unsigned char>(color.z)};
			}
			else
			{
				screen[k * screen_width + i] = { 0, 15, 30 };
			}
		}


	}
}

void Scene::calc_direct_light(Vec3& color, Intersection& record) const
{
	Vec3 albedo = materials.at(record.material).albedo;
	float glossiness = materials.at(record.material).glossiness;
	float specular = materials.at(record.material).specular;

	Vec3 h = normalize(Vec3{ 0.f, 0.f, -1.f } - sunlight.direction);
	color += sunlight.light * max(dot(record.norm, -sunlight.direction), 0) * albedo +
		sunlight.light * pow(max(dot(h, record.norm), 0), glossiness) * specular;
}

void Scene::calc_point_lights(Vec3& color, Intersection& record) const
{
	Vec3 albedo = materials.at(record.material).albedo;
	float glossiness = materials.at(record.material).glossiness;
	float specular = materials.at(record.material).specular;
	
	for (const PointLight& plight : point_lights)
	{
		Vec3 light_dir = plight.position - record.point;
		float light_dist = length(light_dir);
		light_dir = normalize(light_dir);


		Vec3 h = normalize(Vec3{ 0.f, 0.f, -1.f } + light_dir);

		color += plight.light * max(dot(record.norm, light_dir), 0) * albedo / pow(light_dist / plight.light_distance, 2)
			+ plight.light * pow(max(dot(h, record.norm), 0), glossiness) * specular;
	}
}

void Scene::calc_spotlights(Vec3& color, Intersection& record) const
{
	Vec3 albedo = materials.at(record.material).albedo;
	float glossiness = materials.at(record.material).glossiness;
	float specular = materials.at(record.material).specular;

	for (const Spotlight& spotlight : spotlights)
	{
		Vec3 light_dir = spotlight.position - record.point;
		float light_dist = length(light_dir);
		light_dir = normalize(light_dir);


		Vec3 h = normalize(Vec3{ 0.f, 0.f, -1.f } + light_dir);

		float cos = dot(spotlight.direction, -light_dir);
		float intensity;
		if (cos > spotlight.cutOff)
		{
			intensity = 1;
		}
		else if (cos > spotlight.outerCutOff)
		{
			intensity = (cos - spotlight.outerCutOff) / (spotlight.cutOff - spotlight.outerCutOff);
			intensity = 3 * pow(intensity, 2) - 2 * pow(intensity, 3);
		}
		else
		{
			intensity = 0;
		}

		color += intensity * (spotlight.light * max(dot(record.norm, light_dir), 0) * albedo / pow(light_dist / spotlight.light_distance, 2)
			+ spotlight.light * pow(max(dot(h, record.norm), 0), glossiness) * specular);

	}
}
