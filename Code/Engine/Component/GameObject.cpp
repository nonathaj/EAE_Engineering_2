
#include "GameObject.h"
#include "IComponent.h"

namespace Engine
{
	GameObject::GameObject() :
		position_(0.0f, 0.0f, 0.0f),
		rotation_(Quaternion::identity),
		name_(""),
		enabled_(true),
		components_()
	{
	}

	void GameObject::Move(const Engine::Vector3& i_movement)
	{
		position_ += i_movement;
	}

	void GameObject::Rotate(const Engine::Quaternion& i_rotation)
	{
		rotation_ = rotation_ * i_rotation;
	}

	void GameObject::Update(float deltaTime)
	{
		for (size_t x = 0; x < components_.size(); x++)
			components_[x]->Update(deltaTime);
	}
}
