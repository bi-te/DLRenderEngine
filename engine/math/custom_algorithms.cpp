#include "custom_algorithms.h"
#include <random>
#include <chrono>

std::vector<vec3f> fibonacci_set(uint32_t number, float start_phi)
{
	std::vector<vec3f> set;

	float dy = 1.f / number;
	float y = 1.f - dy / 2.f;

	vec3f point;
	float radius, phi = start_phi;
	for (uint32_t i = 0; i < number; ++i)
	{
		point.y() = y;

		radius = sqrtf(1.f - y * y);

		point.x() = cosf(phi) * radius;
		point.z() = sinf(phi) * radius;

		y -= dy;
		phi += GOLDEN_ANGLE;
		set.push_back(point);
	}

	return set;
}

void fibonacci_set(std::vector<vec3f>& set, float start_phi)
{
	float dy = 1.f / set.size();
	float y = 1.f - dy / 2.f;

	vec3f point;
	float radius, phi = start_phi;
	for (uint32_t i = 0; i < set.size(); ++i)
	{
		point.y() = y;

		radius = sqrtf(1.f - y * y);

		point.x() = cosf(phi) * radius;
		point.z() = sinf(phi) * radius;

		y -= dy;
		phi += GOLDEN_ANGLE;
		set.at(i) = point;
	}

}

vec3f fibonacci_set_point(uint32_t number, float start_phi, uint32_t index)
{
	vec3f point;

	point.y() = 1.f - (index + 0.5f) / number;
	float radius = sqrtf(1.f - point.y() * point.y());
	float phi = start_phi + GOLDEN_ANGLE * index;

	point.x() = cosf(phi) * radius;
	point.z() = sinf(phi) * radius;
	return point;
}

void onb_frisvad(vec3f& b1, const vec3f& normal, vec3f& b2)
{
	float sign = copysignf(1.0f, normal.z());
	const float a = -1.0f / (sign + normal.z());
	const float b = normal.x() * normal.y() * a;
	b1 = vec3f(1.0f + sign * normal.x() * normal.x() * a, sign * b, -sign * normal.x());
	b2 = vec3f(b, sign + normal.y() * normal.y() * a, -normal.y());
}

void onb_frisvad(mat3f& basis)
{
	auto n = basis.row(1);
	float sign = copysignf(1.0f, n.z());
	const float a = -1.0f / (sign + n.z());
	const float b = n.x() * n.y() * a;
	basis.row(0) = vec3f(1.0f + sign * n.x() * n.x() * a, sign * b, -sign * n.x());
	basis.row(2) = -vec3f(b, sign + n.y() * n.y() * a, -n.y());
}

vec3f closest_sphere_direction(const vec3f& sphere_rel_pos, const vec3f& sphere_dir, const vec3f& reflection,
	float sphere_dist, float radius, float cos_sphere)
{
	float cosRoS = reflection.dot(sphere_dir);

	if (cosRoS >= cos_sphere) return reflection;
	if (cosRoS < 0.f) return sphere_dir;

	vec3f closes_point_dir = (reflection * sphere_dist * cosRoS - sphere_rel_pos).normalized();
	return (sphere_rel_pos + closes_point_dir * radius).normalized();
}

std::vector<vec2f> poisson_float_2d(float width, float height, float radius, vec2f center, uint32_t k) {
	std::uniform_real_distribution<float> uniform_real(0.f, std::nextafterf(1.f, 2.f));
	std::default_random_engine engine(std::chrono::system_clock::now().time_since_epoch().count());

	std::vector<vec2f> points{};
	std::vector<vec2f> active{};

	width += center.x();
	height += center.y();
	float cell_size = radius / std::sqrtf(2);

	uint32_t n_cells_x = std::ceilf(width / cell_size);
	uint32_t n_cells_y = std::ceilf(height / cell_size);

	std::vector<int32_t> grid(n_cells_x * n_cells_y, -1);

	auto insert_point_to_grid = [&grid, cell_size, n_cells_x](vec2f point, int32_t index) -> void {
		uint32_t x = std::floorf(point.x() / cell_size);
		uint32_t y = std::floorf(point.y() / cell_size);
		grid[y * n_cells_x + x] = index;
	};

	auto is_valid_point = [&grid, &points, width, height, n_cells_x, n_cells_y, radius, cell_size](vec2f point) -> bool {
		if (point.x() < 0 || point.x() > width || point.y() < 0 || point.y() > height) return false;

		uint32_t x_pos = std::floorf(point.x() / cell_size);
		uint32_t y_pos = std::floorf(point.y() / cell_size);

		uint32_t x_min = std::clamp<int>(x_pos - 2, 0, n_cells_x);
		uint32_t x_max = std::clamp<int>(x_pos + 2, 0, n_cells_x);
		uint32_t y_min = std::clamp<int>(y_pos - 2, 0, n_cells_y);
		uint32_t y_max = std::clamp<int>(y_pos + 2, 0, n_cells_y);

		for (uint32_t x = x_min; x < x_max; x++)
			for (uint32_t y = y_min; y < y_max; y++)
				if (grid[y * n_cells_x + x] != -1)
					if ((point - points[grid[y * n_cells_x + x]]).norm() < radius)
						return false;

		return true;
	};

	vec2f p0{ uniform_real(engine) * width , uniform_real(engine) * height };
	p0 += center;

	points.push_back(p0);
	active.push_back(p0);
	insert_point_to_grid(p0, 0);

	while (!active.empty()) {
		uint32_t pointInd = std::floorf(uniform_real(engine) * active.size());
		vec2f point = active[pointInd];

		active.erase(active.begin() + pointInd);

		bool found = false;
		for (uint32_t tryInd = 0; tryInd < k; tryInd++) {
			//generating new point
			float theta = uniform_real(engine) * 2.f * PI;
			float r = uniform_real(engine) * radius + radius;

			vec2f new_point = {
				point.x() + r * cos(theta),
				point.y() + r * sin(theta)
			};

			//checking point
			if (!is_valid_point(new_point)) continue;


			// adding new point
			points.push_back(new_point);
			active.push_back(new_point);
			insert_point_to_grid(new_point, points.size() - 1);
		}

	}

	for (auto& point : points) {
		point += center - vec2f{ width / 2.f, height / 2.f };
	}

	return points;
}
