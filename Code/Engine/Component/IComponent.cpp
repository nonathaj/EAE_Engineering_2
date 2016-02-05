
#include "IComponent.h"
#include "GameObject.h"

namespace Lame
{
	IComponent::IComponent(std::weak_ptr<GameObject> go) : 
		gameObject_(go), enabled_(true)
	{
		gameObject()->components_.push_back(this);
	}

	IComponent::~IComponent()
	{
		if (!gameObject_.expired())
		{
			std::shared_ptr<GameObject> go = gameObject();
			go->components_.erase(std::find(go->components_.begin(), go->components_.end(), this));
		}
	}
}