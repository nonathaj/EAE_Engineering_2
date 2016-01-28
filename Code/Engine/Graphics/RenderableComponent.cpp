
#include "RenderableComponent.h"

#include "Context.h"
#include "../Core/Math.h"

namespace Lame
{
	RenderableComponent::RenderableComponent(std::weak_ptr<Engine::GameObject> go, std::shared_ptr<Mesh> i_mesh, std::shared_ptr<Material> i_material)
		:IComponent(go), mesh_(i_mesh), material_(i_material)
	{
	}

	bool RenderableComponent::Render(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen) const
	{
		std::shared_ptr<Engine::GameObject> go = gameObject();
		if (go)
		{
			//if the gameobject or this component are disabled, we don't need to render anything
			if (!go->enabled() || !enabled())
				return true;

			return material()->Bind() &&							// try to bind the effect
				material()->effect()->SetLocalToWorld(go->transform().LocalToWorld()) &&
				material()->effect()->SetWorldToView(i_worldToView) &&
				material()->effect()->SetViewToScreen(i_viewToScreen) &&
				mesh()->Draw();									// try to draw the mesh
		}
		else
			return false;
	}
}
