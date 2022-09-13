#include "Camera.h"

mat4f invert_to_view(mat4f& src)
{
	mat4f res{};

	res.block<3, 3>(0, 0) = src.block<3, 3>(0, 0).transpose();

	auto pos = src.row(3);

	res.row(3) = -pos(0) * res.row(0) - pos(1) * res.row(1) - pos(2) * res.row(2);
	res.col(3) = vec4f{ 0, 0, 0, 1 };
	return res;
}

mat4f lookAt(const vec3f& position, const vec3f& up, const vec3f& front)
{
	mat4f view = mat4f::Identity();
	vec3f right = up.cross(front).normalized();

	view.row(0).head<3>() = right;
	view.row(1).head<3>() = up;
	view.row(2).head<3>() = front;
	view.transposeInPlace();

	view.row(3).head<3>() = vec3f{
		-position.dot(view.col(0).head<3>()),
		-position.dot(view.col(1).head<3>()),
		-position.dot(view.col(2).head<3>())
	};

	return view;
}

mat4f lookAt(const Transform& transform, const vec3f& front)
{
	mat4f view = mat4f::Identity();
	vec3f up_vec = vec3f{ 0.f, 1.f, 0.f } *transform.rot_matrix;
	vec3f front_vec = front * transform.rot_matrix;
	vec3f right_vec = up_vec.cross(front_vec).normalized();

	view.col(0).head<3>() = right_vec;
	view.col(1).head<3>() = up_vec;
	view.col(2).head<3>() = front_vec;
	view.row(3).head<3>() = vec3f{
		-transform.position().dot(up_vec),
		-transform.position().dot(front_vec),
		-transform.position().dot(right_vec)
	};

	return view;
}

mat4f perspective_proj(float fov, float aspect, float zn, float zf)
{
	mat4f proj = mat4f::Identity();
	proj(1, 1) = 1.f / tanf(fov / 2);
	proj(0, 0) = proj(1, 1) / aspect;
	proj(2, 2) = zn / (zn - zf);
	proj(2, 3) = 1;
	proj(3, 2) = -zf * proj(2, 2);
	proj(3, 3) = 0;
	return proj;
}

void Camera::set_perspective(float fov, float aspect, float zn, float zf)
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

	proj_inv = proj.inverse();
}

void Camera::set_world_angles(const Angles& angles)
{
	basis_update = true;
	rotation = quatf{ Eigen::AngleAxisf{angles.roll, vec3f{0.f, 0.f,1.f}} };
	rotation *= quatf{ Eigen::AngleAxisf{angles.pitch, vec3f{1.f, 0.f,0.f}} };
	rotation *= quatf{ Eigen::AngleAxisf{angles.yaw, vec3f{0.f, 1.f,0.f}} };

	rotation.normalize();
}

void Camera::add_world_angles(const Angles& angles)
{
	basis_update = true;
	rotation *= quatf{ Eigen::AngleAxisf{angles.roll, vec3f{0.f, 0.f,1.f}} };
	rotation *= quatf{ Eigen::AngleAxisf{angles.pitch, vec3f{1.f, 0.f,0.f}} };
	rotation *= quatf{ Eigen::AngleAxisf{angles.yaw, vec3f{0.f, 1.f,0.f}} };

	rotation.normalize();
}


void Camera::add_relative_angles(const Angles& angles)
{
	basis_update = true;

	if(fps_camera)
	{
		//FPS
		rotation *= quatf{ Eigen::AngleAxisf{angles.pitch, right()} };
		rotation *= quatf{ Eigen::AngleAxisf{angles.yaw, vec3f{0.f, 1.f,0.f}}};
	}else
	{
		//spaceship
		rotation *= quatf{ Eigen::AngleAxisf{angles.roll, forward()} };
		rotation *= quatf{ Eigen::AngleAxisf{angles.pitch, right()} };
		rotation *= quatf{ Eigen::AngleAxisf{angles.yaw, up()} };

	}



	rotation.normalize();
}

void Camera::update_basis()
{
	if (!basis_update) return;
	basis_update = false;
	view_inv.block<3, 3>(0, 0) = rotation.toRotationMatrix();
}

void Camera::update_matrices()
{
	if (!matrices_update) return;
	matrices_update = false;

	update_basis();
	view = invert_to_view(view_inv);

	view_proj = view * proj;
	view_proj_inv = proj_inv * view_inv;

	update_frustum_points();
}

void Camera::update_frustum_points()
{
	blnear_fpoint = vec4f{ -1, -1, 1, 1 } * view_proj_inv;
	blnear_fpoint /= blnear_fpoint.w();

	vec4f tlnear_fpoint = vec4f{ -1, 1, 1, 1 } * view_proj_inv;
	tlnear_fpoint /= tlnear_fpoint.w();
	frustrum_up = tlnear_fpoint - blnear_fpoint;

	vec4f brnear_fpoint = vec4f{ 1, -1, 1, 1 } * view_proj_inv;
	brnear_fpoint /= brnear_fpoint.w();
	frustrum_right = brnear_fpoint - blnear_fpoint;
}

void Camera::change_aspect(float asp)
{
	aspect = asp;
	proj(1, 1) = 1.f / tanf(fov / 2);
	proj(0, 0) = proj(1, 1) / asp;

	proj_inv = proj.inverse();
}

void Camera::change_fov(float fovy)
{
	fov = fovy;
	proj(1, 1) = 1.f / tanf(fovy / 2);
	proj(0, 0) = proj(1, 1) / aspect;

	proj_inv = proj.inverse();
}

void Camera::change_znear(float near)
{
	zn = near;
	proj(2, 2) = zn / (zn - zf);
	proj(3, 2) = -zf * proj(2, 2);
}

void Camera::change_zfar(float far)
{
	zf = far;
	proj(2, 2) = zn / (zn - zf);
	proj(3, 2) = -zf * proj(2, 2);
}
