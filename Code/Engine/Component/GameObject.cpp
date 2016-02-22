
#include "GameObject.h"
#include "IComponent.h"

namespace Lame
{
	GameObject::GameObject() :
		transform_(Vector3::zero, Quaternion::identity, Vector3::one),
		name_(""),
		enabled_(true),
		components_()
	{
	}

	void GameObject::Update(float deltaTime)
	{
		for (size_t x = 0; x < components_.size(); x++)
			components_[x]->Update(deltaTime);
	}

	void GameObject::PhysicsUpdate(float deltaTime)
	{
		for (size_t x = 0; x < components_.size(); x++)
			components_[x]->PhysicsUpdate(deltaTime);
	}
}
