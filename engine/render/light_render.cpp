#include "light_render.h"

#include <algorithm>
#include "../math/Ray.h"

float smoothstep(float edge0, float edge1, float x)
{
	x = std::clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
	return x * x * (3.f - 2.f * x);
}

void calc_direct_light(vec3& color, const DirectLight& dirlight, const Intersection& record, const vec3& camera_pos,
                       const Material& m)
{
	float visibility = std::max(record.norm.dot(-dirlight.direction), 0.f);
	if (visibility == 0.f) return;

	vec3 cameravec = (camera_pos - record.point).normalized();
	vec3 h = (cameravec - dirlight.direction).normalized();

	color += dirlight.light.cwiseProduct( visibility * m.albedo) +
		dirlight.light * powf(std::max(h.dot(record.norm), 0.f), m.glossiness) * m.specular;
}

void calc_point_light(vec3& color, const PointLight& plight, const Intersection& record, const vec3& camera_pos,
	const Material& m)
{
	vec3 light_dir = plight.position - record.point;
	float light_dist = light_dir.norm();
	light_dir.normalize();

	float visibility = std::max(record.norm.dot(light_dir), 0.f);
	if (visibility == 0.f) return;

	vec3 cameravec = (camera_pos - record.point).normalized();
	vec3 h = (cameravec + light_dir).normalized();

	color += plight.light.cwiseProduct( visibility * m.albedo) / powf(light_dist / plight.light_range, 2.f)
		+ plight.light * powf(std::max(h.dot(record.norm), 0.f), m.glossiness) * m.specular;
}

void calc_spotlight(vec3& color, const Spotlight& spotlight, const Intersection& record, const vec3& camera_pos,
	const Material& m)
{
	vec3 light_dir = spotlight.position - record.point;
	float light_dist = light_dir.norm();
	light_dir.normalize();

	float cos = spotlight.direction.dot(-light_dir);
	float intensity = smoothstep(spotlight.outerCutOff, spotlight.cutOff, cos);
	if (intensity == 0.f) return;

	vec3 cameravec = (camera_pos - record.point).normalized();
	vec3 h = (cameravec + light_dir).normalized();

	color += intensity * (spotlight.light.cwiseProduct(std::max(record.norm.dot(light_dir), 0.f) * m.albedo) / powf(light_dist / spotlight.light_range, 2.f)
		+ spotlight.light * powf(std::max(h.dot(record.norm), 0.f), m.glossiness) * m.specular);
}
