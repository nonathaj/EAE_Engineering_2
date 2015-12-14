
#include "EnemyComponent.h"

#include "../../Engine/Component/GameObject.h"
#include "../../Engine/Core/Vector3.h"

EnemyComponent::EnemyComponent(std::shared_ptr<Lame::RenderableComponent> i_renderable)
	:IComponent(i_renderable->gameObject()), renderable_(i_renderable), speed(4.0f)
{
}


EnemyComponent::~EnemyComponent()
{
}

void EnemyComponent::Update(float deltaTime)
{
	using namespace Engine;

	Vector3 hereToCenter = -gameObject()->position().normalized();
	gameObject()->Move(hereToCenter * get_speed() * deltaTime);

	gameObject()->Rotate(Engine::Quaternion(static_cast<float>(Engine::Math::ToRadians(30.0f)) * deltaTime, Engine::Vector3::forward));
}
