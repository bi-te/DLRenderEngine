#include "Transform.h"

void Transform::update()
{
	if (!update_transform) return;
	update_transform = false;
	
	rot_matrix = rotation_.toRotationMatrix();

	mtransform.row(0).head<3>() = rot_matrix.row(0) * scale_(0);
	mtransform.row(1).head<3>() = rot_matrix.row(1) * scale_(1);
	mtransform.row(2).head<3>() = rot_matrix.row(2) * scale_(2);
	mtransform.row(3).head<3>() = position_;

	mtransform_inv = mtransform.inverse();
	normal_matrix = mtransform_inv.topLeftCorner<3, 3>().transpose();
}

void Transform::set_world_rotation(const Angles& angles)
{
	rotation_ = quat{ Eigen::AngleAxisf{angles.roll, vec3{0.f, 0.f,1.f}} };
	rotation_ *= quat{ Eigen::AngleAxisf{angles.pitch, vec3{1.f, 0.f,0.f}} };
	rotation_ *= quat{ Eigen::AngleAxisf{angles.yaw, vec3{0.f, 1.f,0.f}} };

	rotation_.normalize();
	update_transform = true;
}

void Transform::add_world_rotation(const Angles& angles)
{
	rotation_ *= quat{ Eigen::AngleAxisf{angles.roll, vec3{0.f, 0.f,1.f}} };
	rotation_ *= quat{ Eigen::AngleAxisf{angles.pitch, vec3{1.f, 0.f,0.f}} };
	rotation_ *= quat{ Eigen::AngleAxisf{angles.yaw, vec3{0.f, 1.f,0.f}} };

	rotation_.normalize();
	update_transform = true;
}

void Transform::add_relative_rotation(const Angles& angles)
{
	rotation_ *= quat{ Eigen::AngleAxisf{angles.roll, rot_matrix.row(2)}};
	rotation_ *= quat{ Eigen::AngleAxisf{angles.pitch, rot_matrix.row(0)} };
	rotation_ *= quat{ Eigen::AngleAxisf{angles.yaw, rot_matrix.row(1)} };

	update_transform = true;
}
