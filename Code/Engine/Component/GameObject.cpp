
#include "GameObject.h"
#include "IComponent.h"

namespace Engine
{
	GameObject::GameObject() :
		position_(0.0f, 0.0f, 0.0f),
		name_(""),
		enabled_(true),
		components_()
	{
	}

	void GameObject::Move(const eae6320::Math::cVector& i_movement)
	{
		position_ += i_movement;
	}

	void GameObject::Update(float deltaTime)
	{
		for (size_t x = 0; x < components_.size(); x++)
			components_[x]->Update(deltaTime);
	}
}