#include "BulletComponent.h"
#include "../../Engine/Core/Vector3.h"

BulletComponent::BulletComponent(std::weak_ptr<Lame::RenderableComponent> i_renderable)
	:IComponent(i_renderable.lock()->gameObject()), renderable_(i_renderable)
{
}


BulletComponent::~BulletComponent()
{
}

void BulletComponent::Update(float i_deltaTime)
{
	gameObject()->Move(gameObject()->rotation() * Engine::Vector3(3.0f, 0.0f, 0.0f) * i_deltaTime);
}
