#ifndef _GAME_PLANETCOMPONENT_H
#define _GAME_PLANETCOMPONENT_H

#include "../../Engine/Component/IComponent.h"
#include "../../Engine/Graphics/RenderableComponent.h"

class PlanetComponent : public Engine::IComponent
{
public:
	static PlanetComponent* Create(std::shared_ptr<Lame::Context> i_graphics_context);
	~PlanetComponent();

	void Update(float deltaTime);

	std::shared_ptr<Lame::RenderableComponent> renderable() { return renderable_; }
private:
	PlanetComponent(std::weak_ptr<Engine::GameObject> i_gameObject);
	std::shared_ptr<Lame::RenderableComponent> renderable_;
	std::shared_ptr<Engine::GameObject> go_;
};

#endif //_GAME_PLANETCOMPONENT_H
