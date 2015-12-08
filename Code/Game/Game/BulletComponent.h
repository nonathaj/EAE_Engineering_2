#ifndef _GAME_BULLETCOMPONENT_H
#define _GAME_BULLETCOMPONENT_H

#include "../../Engine/Component/IComponent.h"
#include "../../Engine/Graphics/RenderableComponent.h"

class BulletComponent : public Engine::IComponent
{
	ADD_TYPEID()
public:
	BulletComponent(float i_creation_time, float i_speed, std::shared_ptr<Lame::RenderableComponent> i_renderable);
	~BulletComponent();

	void Update(float i_deltaTime);

	float get_speed() { return speed; }
	void set_speed(const float& i_speed) { speed = i_speed; }

	float get_creation_time() { return creation_time; }

	std::shared_ptr<Lame::RenderableComponent> renderable() { return renderable_; }
private:
	std::shared_ptr<Lame::RenderableComponent> renderable_;
	float speed;
	float creation_time;
};

#endif //_GAME_BULLETCOMPONENT_H
