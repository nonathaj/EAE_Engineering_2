
#include "BulletComponent.h"
#include "../../Engine/Core/Vector3.h"
#include "../../Engine/Core/Quaternion.h"
#include "../../Engine/Core/Math.h"

BulletComponent::BulletComponent(float i_creation_time, float i_speed, std::shared_ptr<Lame::RenderableComponent> i_renderable) :
	IComponent(i_renderable->gameObject()), 
	renderable_(i_renderable), 
	speed(i_speed), 
	creation_time(i_creation_time)
{
}


BulletComponent::~BulletComponent()
{
}

void BulletComponent::Update(float i_deltaTime)
{
	gameObject()->transform().Move(gameObject()->transform().rotation() * Engine::Vector3(get_speed(), 0.0f, 0.0f) * i_deltaTime);
}
