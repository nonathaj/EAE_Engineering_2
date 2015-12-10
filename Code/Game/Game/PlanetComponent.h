#ifndef _GAME_PLANETCOMPONENT_H
#define _GAME_PLANETCOMPONENT_H

#include "../../Engine/Component/IComponent.h"
#include "../../Engine/Graphics/RenderableComponent.h"
#include "../../Engine/Component/World.h"

class PlanetComponent : public Engine::IComponent
{
public:
	static PlanetComponent* Create(std::shared_ptr<Lame::Context> i_graphics_context, Engine::World *i_world);
	~PlanetComponent();

	void Update(float deltaTime);

	std::shared_ptr<Lame::RenderableComponent> renderable() { return renderable_; }

	float get_weapon_cooldown() { return weapon_cooldown; }
	void set_weapon_cooldown(const float& i_weapon_cooldown) { weapon_cooldown = i_weapon_cooldown; }

	bool can_fire() { return can_fire_timer < 0.0f; }
	void OnFire() { can_fire_timer = weapon_cooldown; }

	float get_rotation_degrees_per_second() { return rotation_degrees_per_second; }
	void set_rotation_degrees_per_second(const float& i_rotation_degrees_per_second) { rotation_degrees_per_second = i_rotation_degrees_per_second; }

private:
	PlanetComponent(std::weak_ptr<Engine::GameObject> i_gameObject);
	std::shared_ptr<Lame::RenderableComponent> renderable_;
	float weapon_cooldown;
	float can_fire_timer;
	float rotation_degrees_per_second;
};

#endif //_GAME_PLANETCOMPONENT_H
