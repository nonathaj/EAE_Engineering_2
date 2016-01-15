
#include "PlanetComponent.h"

#include "../../Engine/Core/Math.h"
#include "../../Engine/System/UserInput.h"
#include "../../Engine/Component/GameObject.h"
#include "../../Engine/Core/Vector3.h"
#include "../../Engine/Core/Quaternion.h"

PlanetComponent* PlanetComponent::Create(std::shared_ptr<Lame::Context> i_graphics_context, Engine::World *i_world)
{
	std::shared_ptr<Lame::Mesh> mesh(Lame::Mesh::Create(i_graphics_context, "data/asteroid.mesh.bin"));
	if (!mesh)
		return nullptr;

	std::shared_ptr<Lame::Material> material(Lame::Material::Create(i_graphics_context, "data/asteroid.material.bin"));
	if (!material)
		return nullptr;

	std::shared_ptr<Engine::GameObject> go = i_world->AddNewGameObject();
	if (!go)
		return nullptr;

	std::shared_ptr<Lame::RenderableComponent> renderable(new Lame::RenderableComponent(go, mesh, material));
	if (!renderable)
	{
		i_world->Remove(go);
		return nullptr;
	}

	PlanetComponent* planet = new PlanetComponent(go);
	if (!planet)
	{
		i_world->Remove(go);
		return nullptr;
	}

	planet->renderable_ = renderable;
	return planet;
}

PlanetComponent::PlanetComponent(std::weak_ptr<Engine::GameObject> i_gameObject) :
	IComponent(i_gameObject), can_fire_timer(0.0f), weapon_cooldown(1.0f), rotation_degrees_per_second(90.0f)
{
}


PlanetComponent::~PlanetComponent()
{
}

void PlanetComponent::Update(float deltaTime)
{
	using namespace System::UserInput;
	using namespace Engine;

	can_fire_timer -= deltaTime;

	const float rotationSpeed = static_cast<float>(Math::ToRadians(rotation_degrees_per_second * deltaTime));
	Vector3 rotationAxis(0.0f, 0.0f, 1.0f);
	std::shared_ptr<Engine::GameObject> go = gameObject();
	if (Keyboard::Pressed(Keyboard::A))
		go->Rotate(Quaternion(-rotationSpeed, rotationAxis));
	else if (Keyboard::Pressed(Keyboard::D))
		go->Rotate(Quaternion(rotationSpeed, rotationAxis));

	float movementAmount = 3.0f * deltaTime;
	if (Keyboard::Pressed(Keyboard::I))						//up
		go->Move(Vector3(0.0f, movementAmount, 0.0f));
	if (Keyboard::Pressed(Keyboard::K))						//down
		go->Move(Vector3(0.0f, -movementAmount, 0.0f));
	if (Keyboard::Pressed(Keyboard::L))						//right
		go->Move(Vector3(movementAmount, 0.0f, 0.0f));
	if (Keyboard::Pressed(Keyboard::J))						//left
		go->Move(Vector3(-movementAmount, 0.0f, 0.0f));
}
