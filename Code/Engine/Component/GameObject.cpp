
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
}