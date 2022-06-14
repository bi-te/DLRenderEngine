#pragma once

#include "math.h"

class Transform
{
	quat rotation_ = quat::Identity();
	vec3 position_{0, 0, 0};
	vec3 scale_{1, 1, 1};

	bool updated;
public:
	mat3 normal_matrix = mat3::Identity();
	mat4 mtransform = mat4::Identity(),
		 mtransform_inv = mat4::Identity();

	bool is_updated() const { return updated; }
	const quat& rotation() const { return rotation_; }
	const vec3& position() const { return position_; }
	const vec3& scale() const { return scale_; }

	void update();

	void set_scale(const vec3& sc){ scale_ = sc; updated = true; }
	void scale_up(float s){ scale_ *= s; updated = true; }
	void scale_up(const vec3& s){ scale_ = scale_.cwiseProduct(s); updated = true; }

	void set_rotation(const quat& rot) { rotation_ = rot;  updated = true; }
	void add_rotation(const quat& rot) { rotation_ *= rot; updated = true; }

	void set_world_rotation(const Angles& angles);
	void add_world_rotation(const Angles& angles);

	void set_world_offset(const vec3& offset) { position_ = offset;  updated = true; }
	void add_world_offset(const vec3& offset) { position_ += offset; updated = true; }
	
};