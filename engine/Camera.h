#pragma once

#include "math/math.h"

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

	auto right() { return view_inv.row(0).head<3>(); }
	auto up() { return view_inv.row(1).head<3>(); }
	auto forward() { return view_inv.row(2).head<3>(); }
	auto position() { return view_inv.row(3).head<3>(); }



	void set_perspective(float fov, float aspect, float zn, float zf)
	{
		this->fov = fov;
		this->aspect = aspect;
		this->zn = zn;
		this->zf = zf;

		proj(1, 1) = 1.f / tanf(fov / 2);
		proj(0, 0) = proj(1, 1) / aspect;
		proj(2, 2) = zn / (zn - zf);
		proj(2, 3) = 1;
		proj(3, 2) = -zf * proj(2, 2);
		proj(3, 3) = 0;

		float a = proj(2, 2);

		proj_inv = proj.inverse();
	}



	void add_relative_offset(const vec3& offset)
	{
		position() +=
			right() * offset.x() +
			up() * offset.y() +
			forward() * offset.z();
	}

	void add_relative_angles(const Angles& angles)
	{
		update_basis();

		rotation *= quat{ Eigen::AngleAxisf{angles.roll, forward()}};
		rotation *= quat{ Eigen::AngleAxisf{angles.pitch, right()}};
		rotation *= quat{ Eigen::AngleAxisf{angles.yaw, up()}};

		rotation.normalize();
	}

	void update_basis()
	{
		view_inv.block<3, 3>(0, 0) = rotation.toRotationMatrix();
	}

	void update_matrices()
	{
		update_basis();
		view = view_inv.inverse();

		view_proj = view * proj;
		view_proj_inv = proj_inv * view_inv;
	}

	void change_aspect(float asp)
	{
		aspect = asp;
		proj(1, 1) = 1.f / tanf(fov / 2);
		proj(0, 0) = proj(1, 1) / asp;

		proj_inv = proj.inverse();
	}

	void change_fov(float fovy)
	{
		fov = fovy;
		proj(1, 1) = 1.f / tanf(fovy / 2);
		proj(0, 0) = proj(1, 1) / aspect;

		proj_inv = proj.inverse();
	}

};

