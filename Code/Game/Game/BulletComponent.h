#ifndef _GAME_BULLETCOMPONENT_H
#define _GAME_BULLETCOMPONENT_H

#include "../../Engine/Component/IComponent.h"
#include "../../Engine/Graphics/RenderableComponent.h"

class BulletComponent : public Engine::IComponent
{
	ADD_TYPEID()
public:
	BulletComponent(std::weak_ptr<Lame::RenderableComponent> i_renderable);
	~BulletComponent();

	void Update(float i_deltaTime);

	std::weak_ptr<Lame::RenderableComponent> renderable() { return renderable_; }
private:
	std::weak_ptr<Lame::RenderableComponent> renderable_;
};

#endif //_GAME_BULLETCOMPONENT_H
