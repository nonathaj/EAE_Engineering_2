
#include "Gameplay.h"

#include <vector>
#include <string>

#include "../../Engine/Graphics/Context.h"
#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/Effect.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Core/Singleton.h"
#include "../../Engine/System/eae6320/Time.h"
#include "../../Engine/Component/World.h"
#include "../../Engine/Component/GameObject.h"
#include "../../Engine/Graphics/RenderableComponent.h"
#include "../../Engine/System/UserInput.h"
#include "../../Engine/System/Console.h"
#include "../../Engine/System/UserOutput.h"
#include "../../Engine/Core/Math.h"

#include "BulletComponent.h"

namespace
{
	std::shared_ptr<Lame::Material> CreateMaterial(std::string i_material);
	std::shared_ptr<Lame::Mesh> CreateMesh(std::string i_mesh);
	std::shared_ptr<Lame::RenderableComponent> CreateRenderableObject(std::shared_ptr<Lame::Mesh> i_mesh, std::shared_ptr<Lame::Material> i_material);

	std::shared_ptr<BulletComponent> CreateBullet();

	std::unique_ptr<Lame::Graphics> graphics;
	std::unique_ptr<Engine::World> world;

	//gameobjects
	std::shared_ptr<Engine::GameObject> asteroid;
	std::vector<std::shared_ptr<BulletComponent>> bullets;

	//materials
	std::shared_ptr<Lame::Material> bulletMat;

	//meshes
	std::shared_ptr<Lame::Mesh> bulletMesh;

	void HandleInput(float deltaTime);
}

namespace Gameplay
{
	bool Initialize(HWND i_window)
	{
		using namespace Lame;

		//generate container/manager objects
		{
			world = std::unique_ptr<Engine::World>(new Engine::World());
			if (!world)
			{
				Shutdown();
				return false;
			}

			std::shared_ptr<Context> context(Context::Create(i_window));
			if (!context)	//if we failed to generate a context, shutdown the game
			{
				Shutdown();
				return false;
			}
			graphics = std::unique_ptr<Graphics>(Graphics::Create(context));
			if (!graphics) //if we failed to generate a graphics object, shutdown the game
			{
				Shutdown();
				return false;
			}

			std::string error;
			if (!eae6320::Time::Initialize(&error))
			{
				System::UserOutput::Display(error, "Time initialization error");
				Shutdown();
				return false;
			}
		}

		//initial camera position
		graphics->camera()->gameObject()->position(Engine::Vector3(0, 0, 15));

		//Create our Materials


		//Create our Meshes


		//Create our renderables
		asteroid = CreateRenderableObject(CreateMesh("data/asteroid.mesh.bin"), CreateMaterial("data/asteroid.material.bin"))->gameObject();

		if (!asteroid)
		{
			Shutdown();
			return false;
		}

		return true;
	}

	bool RunFrame()
	{
		eae6320::Time::OnNewFrame();
		float deltaTime = eae6320::Time::GetSecondsElapsedThisFrame();

		HandleInput(deltaTime);

		world->Update(deltaTime);
		bool renderSuccess = graphics->Render();
		return renderSuccess;
	}

	bool Shutdown()
	{
		asteroid.reset();

		bulletMat.reset();
		bulletMesh.reset();
		bullets.clear();

		world.reset();
		graphics.reset();
		return true;
	}
}

namespace
{
	void HandleInput(float deltaTime)
	{
		using namespace System::UserInput;
		using namespace Engine;

		//Rotate asteroid
		const float rotationSpeed = static_cast<float>(Engine::Math::ToRadians(20.0f * deltaTime));
		Vector3 rotationAxis(0.0f, 0.0f, 1.0f);
		if (Keyboard::Pressed(Keyboard::A))
			asteroid->Rotate(Quaternion(-rotationSpeed, rotationAxis));
		else if (Keyboard::Pressed(Keyboard::D))
			asteroid->Rotate(Quaternion(rotationSpeed, rotationAxis));

		//fire bullet
		if (Keyboard::Pressed(Keyboard::Space))
		{
			const Vector3 spawnPosition = asteroid->position() + asteroid->rotation() * Vector3(2.5f, 0.0f, 0.0f);
			std::shared_ptr<BulletComponent> bullet = CreateBullet();
			bullet->gameObject()->position(spawnPosition);
			bullet->gameObject()->rotation(asteroid->rotation());
		}
	}

	std::shared_ptr<BulletComponent> CreateBullet()
	{
		if (!bulletMat)
		{
			bulletMat = CreateMaterial("bullet.material.bin");
			if (!bulletMat)
				return nullptr;
		}

		if (!bulletMesh)
		{
			bulletMesh = CreateMesh("bullet.mesh.bin");
			if (!bulletMesh)
				return nullptr;
		}

		std::shared_ptr<Lame::RenderableComponent> renderable = CreateRenderableObject(bulletMesh, bulletMat);
		if (!renderable)
			return nullptr;

		std::shared_ptr<BulletComponent> bullet = std::shared_ptr<BulletComponent>(new BulletComponent(renderable));
		if (!bullet)
		{
			return nullptr;
		}
		bullet->gameObject()->enabled(false);
		bullets.push_back(bullet);
		return bullet;
	}

	std::shared_ptr<Lame::Material> CreateMaterial(std::string i_material)
	{
		if (!graphics || !graphics->context())
			return nullptr;

		using namespace Lame;
		return std::shared_ptr<Material>(Material::Create(graphics->context(), i_material));
	}

	std::shared_ptr<Lame::Mesh> CreateMesh(std::string i_mesh)
	{
		if (!graphics || !graphics->context())
			return nullptr;

		using namespace Lame;
		return std::shared_ptr<Mesh>(Mesh::Create(graphics->context(), i_mesh));
	}

	std::shared_ptr<Lame::RenderableComponent> CreateRenderableObject(std::shared_ptr<Lame::Mesh> i_mesh, std::shared_ptr<Lame::Material> i_material)
	{
		if (!world || !graphics || !graphics->context() || !i_mesh || !i_material)
			return nullptr;

		using namespace Lame;

		//create the gameObject
		std::shared_ptr<Engine::GameObject> go(new Engine::GameObject());
		if (!go)
			return nullptr;

		//add the object to the world
		if (!world->Add(go))
			return nullptr;

		//create the renderable
		std::shared_ptr<RenderableComponent> renderable(new RenderableComponent(go, i_mesh, i_material));
		if (!renderable)
		{
			world->Remove(go);
			return nullptr;
		}

		//add the renderable to the graphics system
		if (!graphics->Add(renderable))
		{
			world->Remove(go);
			return nullptr;
		}

		return renderable;
	}
}
