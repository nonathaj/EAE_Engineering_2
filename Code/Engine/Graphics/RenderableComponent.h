#ifndef _ENGINE_GRAPHICS_RENDERABLECOMPONENT_H
#define _ENGINE_GRAPHICS_RENDERABLECOMPONENT_H

#include "../Component/IComponent.h"
#include "../Component/GameObject.h"
#include "Mesh.h"
#include "Material.h"

namespace Lame
{
	class RenderableComponent : public Lame::IComponent
	{
		ADD_TYPEID()
	public: 		
		static RenderableComponent* Create(std::weak_ptr<Lame::GameObject> go, std::shared_ptr<Mesh> i_mesh, std::shared_ptr<Material> i_material);

		bool Render(const Lame::Matrix4x4& i_worldToView, const Lame::Matrix4x4& i_viewToScreen) const;

		bool SetLocalToWorld(const Lame::Matrix4x4& i_matrix) const;
		bool SetWorldToView(const Lame::Matrix4x4& i_matrix) const;
		bool SetViewToScreen(const Lame::Matrix4x4& i_matrix) const;

		inline std::shared_ptr<Mesh> mesh() const { return mesh_; }
		inline std::shared_ptr<Material> material() const { return material_; }
	private:
		RenderableComponent();
		RenderableComponent(std::weak_ptr<Lame::GameObject> go) : IComponent(go) { }

		std::shared_ptr<Mesh> mesh_;
		std::shared_ptr<Material> material_;

		Effect::ConstantHandle localToWorldUniformId;
		Effect::ConstantHandle worldToViewUniformId;
		Effect::ConstantHandle viewToScreenUniformId;

		static char const * const LocalToWorldUniformName;
		static char const * const WorldToViewUniformName;
		static char const * const ViewToScreenUniformName;
	};
}

#endif //_ENGINE_GRAPHICS_RENDERABLECOMPONENT_H
