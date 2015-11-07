
#include "RenderableComponent.h"

#include "Context.h"
#include "../Core/Math.h"

namespace Lame
{
	RenderableComponent::RenderableComponent(std::weak_ptr<Engine::GameObject> go, std::shared_ptr<Mesh> i_mesh, std::shared_ptr<Effect> i_effect)
		:IComponent(go), mesh_(i_mesh), effect_(i_effect)
	{
	}

	bool RenderableComponent::Render()
	{
		using namespace eae6320::Math;

		std::shared_ptr<Engine::GameObject> go = gameObject();
		if (go)
		{
			//if the gameobject or this component are disabled, we don't need to render anything
			if (!go->enabled() || !enabled())
				return true;

			cMatrix_transformation localToWorld(go->rotation(), go->position());
			cMatrix_transformation worldToView = cMatrix_transformation::CreateWorldToViewTransform(cQuaternion(), cVector(0, 0, 10));

			float fov = static_cast<float>(Engine::Math::ToRadians(60));
			float aspectRatio = static_cast<float>(mesh()->get_context()->screen_width()) / static_cast<float>(mesh()->get_context()->screen_height());
			cMatrix_transformation viewToScreen = cMatrix_transformation::CreateViewToScreenTransform(fov, aspectRatio, 0.1f, 100.0f);

			return effect()->Bind() &&							// try to bind the effect
				effect()->SetLocalToWorld(localToWorld) &&
				effect()->SetWorldToView(worldToView) &&
				effect()->SetViewToScreen(viewToScreen) &&
				mesh()->Draw();									// try to draw the mesh
		}
		else
			return false;
	}
}
