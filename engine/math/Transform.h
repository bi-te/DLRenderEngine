#pragma once

#include "math.h"

class Transform
{
	quatf rotation_ = quatf::Identity();
	vec3f position_{0, 0, 0};
	vec3f scale_{1, 1, 1};

	bool update_transform;
public:
	mat3f normal_matrix  = mat3f::Identity(),
		  rot_matrix	 = mat3f::Identity();
	mat4f mtransform	 = mat4f::Identity(),
		  mtransform_inv = mat4f::Identity();

	mat4f matrix() { update(); return mtransform; }
	mat4f matrix_inv() { update(); return mtransform_inv; }
	mat3f rotation() { rot_matrix = rotation_.toRotationMatrix(); return rot_matrix; }

	bool is_updated() const { return !update_transform; }
	const quatf& rotation() const { return rotation_; }
	const vec3f& position() const { return position_; }
	const vec3f& scale() const { return scale_; }

	void update();

	void set_scale(float s) { scale_ = vec3f{ s, s, s }; }
	void set_scale(const vec3f& sc){ scale_ = sc; update_transform = true; }
	void scale_up(float s){ scale_ *= s; update_transform = true; }
	void scale_up(const vec3f& s){ scale_ = scale_.cwiseProduct(s); update_transform = true; }

	void set_rotation(const quatf& rot) { rotation_ = rot;  update_transform = true; }
	void add_rotation(const quatf& rot) { rotation_ *= rot; update_transform = true; }

	void set_world_rotation(const Angles& angles);
	void add_world_rotation(const Angles& angles);
	void add_relative_rotation(const Angles& angles);

	void set_world_offset(const vec3f& offset) { position_ = offset;  update_transform = true; }
	void add_world_offset(const vec3f& offset) { position_ += offset; update_transform = true; }
	
};