#include "Transform.h"

namespace Engine
{
	void Transform::Move(const Engine::Vector3& i_movement)
	{
		position_ += i_movement;
	}

	void Transform::Rotate(const Engine::Quaternion& i_rotation)
	{
		rotation_ = rotation_ * i_rotation;
	}
}
