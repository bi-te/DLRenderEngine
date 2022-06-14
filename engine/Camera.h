#pragma once

#include "math/math.h"

mat4 invert_to_view(mat4& src);

class Camera
{
public:

	float fov, aspect, zn, zf;

	mat4 view = mat4::Identity();
	mat4 view_inv = mat4::Identity();
	mat4 proj = mat4::Identity();
	mat4 proj_inv = mat4::Identity();
	mat4 view_proj = mat4::Identity();
	mat4 view_proj_inv = mat4::Identity();

	quat rotation = quat::Identity();
	bool fps_camera = true;

	bool matrices_update = false, basis_update = false;

	vec4 blnear_fpoint = { -1, -1, 1, 1 };
	vec4 frustrum_up = vec4::Identity();
	vec4 frustrum_right = vec4::Identity();

	auto right() { return view_inv.row(0).head<3>(); }
	auto up() { return view_inv.row(1).head<3>(); }
	auto forward() { return view_inv.row(2).head<3>(); }
	auto position() { return view_inv.row(3).head<3>(); }

	const auto position() const{ return view_inv.row(3).head<3>(); }

	void set_perspective(float fov, float aspect, float zn, float zf);

	void set_world_offset(const vec3& offset) { position() = offset;  matrices_update = true; }
	void add_world_offset(const vec3& offset) { position() += offset; matrices_update = true;  }
	void add_relative_offset(const vec3& offset){
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

