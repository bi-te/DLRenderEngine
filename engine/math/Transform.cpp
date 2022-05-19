#include "Transform.h"

void Transform::update()
{
	if (updated)
	{
		mat4 rot = mat4::Identity();
		rot.block<3, 3>(0, 0) = rotation_.toRotationMatrix();

		mtransform.row(0) = rot.row(0) * scale_(0);
		mtransform.row(1) = rot.row(1) * scale_(1);
		mtransform.row(2) = rot.row(2) * scale_(2);
		mtransform.row(3).head<3>() = position_;

		mtransform_inv = mtransform.inverse();
		updated = false;
	}
}

void Transform::set_world_rotation(const Angles& angles)
{
	rotation_ = quat{ Eigen::AngleAxisf{angles.roll, vec3{0.f, 0.f,1.f}} };
	rotation_ *= quat{ Eigen::AngleAxisf{angles.pitch, vec3{1.f, 0.f,0.f}} };
	rotation_ *= quat{ Eigen::AngleAxisf{angles.yaw, vec3{0.f, 1.f,0.f}} };

	rotation_.normalize();
	updated = true;
}

void Transform::add_world_rotation(const Angles& angles)
{
	rotation_ *= quat{ Eigen::AngleAxisf{angles.roll, vec3{0.f, 0.f,1.f}} };
	rotation_ *= quat{ Eigen::AngleAxisf{angles.pitch, vec3{1.f, 0.f,0.f}} };
	rotation_ *= quat{ Eigen::AngleAxisf{angles.yaw, vec3{0.f, 1.f,0.f}} };

	rotation_.normalize();
	updated = true;
}
