#ifndef _GAME_ENEMYCOMPONENT_H
#define _GAME_ENEMYCOMPONENT_H

#include "../../Engine/Component/IComponent.h"
#include "../../Engine/Component/GameObject.h"
#include "../../Engine/Graphics/RenderableComponent.h"

class EnemyComponent : public Engine::IComponent
{
public:
	EnemyComponent(std::shared_ptr<Lame::RenderableComponent> i_renderable);
	~EnemyComponent();

	void Update(float deltaTime);

	float get_speed() { return speed; }
	void set_float(const float& i_speed) { speed = i_speed; }

	std::shared_ptr<Lame::RenderableComponent> renderable() { return renderable_; }
private:
	std::shared_ptr<Lame::RenderableComponent> renderable_;

	float speed;
};

#endif //_GAME_ENEMYCOMPONENT_H