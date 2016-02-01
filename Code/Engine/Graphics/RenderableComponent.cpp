
#include "RenderableComponent.h"

#include "Context.h"
#include "../Core/Math.h"
#include "../System/UserOutput.h"

namespace Lame
{
	char const * const RenderableComponent::LocalToWorldUniformName = "local_to_world";
	char const * const RenderableComponent::WorldToViewUniformName = "world_to_view";
	char const * const RenderableComponent::ViewToScreenUniformName = "view_to_screen";

	RenderableComponent* RenderableComponent::Create(std::weak_ptr<Engine::GameObject> go, std::shared_ptr<Mesh> i_mesh, std::shared_ptr<Material> i_material)
	{
		if (go.expired() || !i_mesh || !i_material)
			return nullptr;

		Effect::ConstantHandle localToWorldUniformId;
		Effect::ConstantHandle worldToViewUniformId;
		Effect::ConstantHandle viewToScreenUniformId;

		if (!i_material->effect()->CacheConstant(Effect::Shader::Vertex, LocalToWorldUniformName, localToWorldUniformId) ||
			!i_material->effect()->CacheConstant(Effect::Shader::Vertex, WorldToViewUniformName, worldToViewUniformId) ||
			!i_material->effect()->CacheConstant(Effect::Shader::Vertex, ViewToScreenUniformName, viewToScreenUniformId) )
		{
			System::UserOutput::Display("Failed to find all required uniform constants for RenderableComponent.");
			return nullptr;
		}

		RenderableComponent* comp = new RenderableComponent(go);
		if (!comp)
			return nullptr;

		comp->mesh_ = i_mesh;
		comp->material_ = i_material;
		comp->localToWorldUniformId = localToWorldUniformId;
		comp->worldToViewUniformId = worldToViewUniformId;
		comp->viewToScreenUniformId = viewToScreenUniformId;
		return comp;
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
				SetLocalToWorld(go->transform().LocalToWorld()) &&
				SetWorldToView(i_worldToView) &&
				SetViewToScreen(i_viewToScreen) &&
				mesh()->Draw();									// try to draw the mesh
		}
		else
			return false;
	}
	
	bool RenderableComponent::SetLocalToWorld(const Engine::Matrix4x4& i_matrix) const
	{
		return material()->effect()->SetConstant(Effect::Shader::Vertex, localToWorldUniformId, i_matrix);
	}

	bool RenderableComponent::SetWorldToView(const Engine::Matrix4x4& i_matrix) const
	{
		return material()->effect()->SetConstant(Effect::Shader::Vertex, worldToViewUniformId, i_matrix);
	}

	bool RenderableComponent::SetViewToScreen(const Engine::Matrix4x4& i_matrix) const
	{
		return material()->effect()->SetConstant(Effect::Shader::Vertex, viewToScreenUniformId, i_matrix);
	}
}
