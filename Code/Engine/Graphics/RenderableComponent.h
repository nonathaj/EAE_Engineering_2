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
		RenderableComponent(std::weak_ptr<Engine::GameObject> go, Mesh* i_mesh, Effect* i_effect);

		bool Render();

		inline Lame::Mesh* mesh() { return mesh_; }
		inline Lame::Effect* effect() { return effect_; }
	private:
		RenderableComponent();

		Mesh* mesh_;
		Effect* effect_;
	};
}

#endif //_ENGINE_GRAPHICS_RENDERABLECOMPONENT_H