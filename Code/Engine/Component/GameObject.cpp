
#include "GameObject.h"
#include "IComponent.h"

namespace Engine
{
	GameObject::GameObject() :
		transform_(Engine::Vector3::zero, Engine::Quaternion::identity),
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
}
