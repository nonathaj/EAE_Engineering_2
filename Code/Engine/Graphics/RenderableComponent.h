#ifndef _ENGINE_GRAPHICS_RENDERABLECOMPONENT_H
#define _ENGINE_GRAPHICS_RENDERABLECOMPONENT_H

#include "../Component/IComponent.h"
#include "../Component/GameObject.h"
#include "Mesh.h"
#include "Effect.h"

namespace Lame
{
	class RenderableComponent : public Engine::IComponent
	{
		ADD_TYPEID()
	public:
		RenderableComponent(std::weak_ptr<Engine::GameObject> go, std::shared_ptr<Mesh> i_mesh, std::shared_ptr<Effect> i_effect);

		bool Render();

		inline std::shared_ptr<Mesh> mesh() { return mesh_; }
		inline std::shared_ptr<Effect> effect() { return effect_; }
	private:
		RenderableComponent();

		std::shared_ptr<Mesh> mesh_;
		std::shared_ptr<Effect> effect_;
	};
}

#endif //_ENGINE_GRAPHICS_RENDERABLECOMPONENT_H
