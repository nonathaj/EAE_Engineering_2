
#include "IComponent.h"
#include "GameObject.h"

namespace Engine
{
	IComponent::IComponent(std::weak_ptr<GameObject> go) : 
		gameObject_(go), enabled_(true)
	{
	}
}