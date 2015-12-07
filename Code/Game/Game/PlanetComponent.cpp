
#include "PlanetComponent.h"

#include "../../Engine/Core/Math.h"
#include "../../Engine/System/UserInput.h"
#include "../../Engine/Component/GameObject.h"
#include "../../Engine/Core/Vector3.h"
#include "../../Engine/Core/Quaternion.h"

PlanetComponent* PlanetComponent::Create(std::shared_ptr<Lame::Context> i_graphics_context)
{
	std::shared_ptr<Engine::GameObject> go = std::shared_ptr<Engine::GameObject>(new Engine::GameObject());
	if (!go)
		return nullptr;

	std::shared_ptr<Lame::Mesh> mesh = std::shared_ptr<Lame::Mesh>(Lame::Mesh::Create(i_graphics_context, "data/asteroid.mesh.bin"));
	if (!mesh)
		return nullptr;

	std::shared_ptr<Lame::Material> material = std::shared_ptr<Lame::Material>(Lame::Material::Create(i_graphics_context, "data/asteroid.material.bin"));
	if (!material)
		return nullptr;

	std::shared_ptr<Lame::RenderableComponent> renderable = std::shared_ptr<Lame::RenderableComponent>(new Lame::RenderableComponent(go, mesh, material));
	if (!renderable)
		return nullptr;

	PlanetComponent* planet = new PlanetComponent(go);
	if (!planet)
		return nullptr;

	planet->renderable_ = renderable;
	planet->go_ = go;
	return planet;
}

PlanetComponent::PlanetComponent(std::weak_ptr<Engine::GameObject> i_gameObject) :
	IComponent(i_gameObject)
{
}


PlanetComponent::~PlanetComponent()
{
}

void PlanetComponent::Update(float deltaTime)
{
	using namespace System::UserInput;
	using namespace Engine;

	const float rotationSpeed = static_cast<float>(Math::ToRadians(20.0f * deltaTime));
	Vector3 rotationAxis(0.0f, 0.0f, 1.0f);
	std::shared_ptr<Engine::GameObject> go = gameObject();
	if (Keyboard::Pressed(Keyboard::A))
		go->Rotate(Quaternion(-rotationSpeed, rotationAxis));
	else if (Keyboard::Pressed(Keyboard::D))
		go->Rotate(Quaternion(rotationSpeed, rotationAxis));
}
