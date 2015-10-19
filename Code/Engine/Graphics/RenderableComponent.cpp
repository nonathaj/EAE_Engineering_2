
#include "RenderableComponent.h"

namespace Lame
{
	RenderableComponent::RenderableComponent(std::weak_ptr<Engine::GameObject> go, std::shared_ptr<Mesh> i_mesh, std::shared_ptr<Effect> i_effect)
		:IComponent(go), mesh_(i_mesh), effect_(i_effect)
	{
	}

	bool RenderableComponent::Render()
	{
		std::shared_ptr<Engine::GameObject> go = gameObject();
		if (go)
		{
			//if the gameobject or this component are disabled, we don't need to render anything
			if (!go->enabled() || !enabled())
				return true;

			return effect()->Bind() &&							// try to bind the effect
				effect()->SetPosition(go->position()) &&		// try to set the position
				mesh()->Draw();									// try to draw the mesh
		}
		else
			return false;
	}
}