#pragma once

#include "math/Vec3.h"

class Camera
{
public:

	float fov, aspect, zn, zf;

	mat4 view = mat4::Identity();
	mat4 view_inv = mat4::Identity();

	mat4 proj = mat4::Identity();
	mat4 proj_inv = mat4::Identity();

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

