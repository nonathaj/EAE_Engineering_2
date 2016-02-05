#include "Transform.h"

namespace Lame
{
	void Transform::Move(const Lame::Vector3& i_movement)
	{
		position_ += i_movement;
	}

	void Transform::Rotate(const Lame::Quaternion& i_rotation)
	{
		rotation_ = rotation_ * i_rotation;
	}
}
