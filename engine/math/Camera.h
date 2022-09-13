#pragma once

#include "math/math.h"
#include "Transform.h"

mat4f invert_to_view(mat4f& src);
mat4f lookAt(const vec3f& position, const vec3f& up, const vec3f& front);
mat4f lookAt(const Transform& transform, const vec3f& front);
mat4f perspective_proj(float fov, float aspect, float zn, float zf);

const vec3f WORLD_X{ 1.f, 0.f, 0.f };
const vec3f WORLD_Y{ 0.f, 1.f, 0.f };
const vec3f WORLD_Z{ 0.f, 0.f, 1.f };

class Camera
{
public:

	float fov, aspect, zn, zf;

	mat4f view = mat4f::Identity();
	mat4f view_inv = mat4f::Identity();
	mat4f proj = mat4f::Identity();
	mat4f proj_inv = mat4f::Identity();
	mat4f view_proj = mat4f::Identity();
	mat4f view_proj_inv = mat4f::Identity();

	quatf rotation = quatf::Identity();
	bool fps_camera = true;

	bool matrices_update = false, basis_update = false;

	vec4f blnear_fpoint = { -1.f, -1.f, 1.f, 1.f };
	vec4f frustrum_up = vec4f::Identity();
	vec4f frustrum_right = vec4f::Identity();

	auto right() { return view_inv.row(0).head<3>(); }
	auto up() { return view_inv.row(1).head<3>(); }
	auto forward() { return view_inv.row(2).head<3>(); }
	auto position() { return view_inv.row(3).head<3>(); }

	const auto position() const{ return view_inv.row(3).head<3>(); }

	void set_perspective(float fov, float aspect, float zn, float zf);

	void set_world_offset(const vec3f& offset) { position() = offset;  matrices_update = true; }
	void add_world_offset(const vec3f& offset) { position() += offset; matrices_update = true;  }
	void add_relative_offset(const vec3f& offset){
		update_basis(); matrices_update = true;
		position() += right() * offset.x() + up() * offset.y() + forward() * offset.z();	
	}

	void set_world_angles(const Angles& angles);
	void add_world_angles(const Angles& angles);
	void add_relative_angles(const Angles& angles);

	void update_basis();
	void update_matrices();
	void update_frustum_points();

	void change_aspect(float asp);
	void change_fov(float fovy);
	void change_znear(float near);
	void change_zfar(float far);

};

