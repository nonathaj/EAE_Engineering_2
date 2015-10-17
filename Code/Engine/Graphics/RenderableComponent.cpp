
#include "RenderableComponent.h"

namespace Lame
{
	RenderableComponent::RenderableComponent(std::weak_ptr<Engine::GameObject> go, Mesh* i_mesh, Effect* i_effect)
		:IComponent(go), mesh_(i_mesh), effect_(i_effect)
	{
	}

	bool RenderableComponent::Render()
	{
		std::shared_ptr<Engine::GameObject> go = gameObject();
		if (go)
		{
			effect()->Bind();
			effect()->SetPosition(go->position());
			mesh()->Draw();
		}
	}
}