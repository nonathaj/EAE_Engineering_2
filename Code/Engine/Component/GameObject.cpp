
#include "GameObject.h"
#include "IComponent.h"

namespace Lame
{
	GameObject::GameObject() :
		transform_(Lame::Vector3::zero, Lame::Quaternion::identity),
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
