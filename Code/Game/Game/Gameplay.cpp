
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
#include "PlanetComponent.h"

namespace
{
	std::shared_ptr<Lame::Material> CreateMaterial(std::string i_material);
	std::shared_ptr<Lame::Mesh> CreateMesh(std::string i_mesh);
	std::shared_ptr<Lame::RenderableComponent> CreateRenderableObject(std::shared_ptr<Lame::Mesh> i_mesh, std::shared_ptr<Lame::Material> i_material);

	std::shared_ptr<BulletComponent> CreateBullet();

	Lame::Graphics *graphics = nullptr;
	Engine::World *world = nullptr;

	std::shared_ptr<PlanetComponent> asteroid;

	std::vector<std::shared_ptr<BulletComponent>> bullets;

	std::shared_ptr<Lame::Material> bulletMat;
	std::shared_ptr<Lame::Mesh> bulletMesh;

	void HandleInput(float deltaTime);
}

namespace Gameplay
{
	bool Initialize(HWND i_window)
	{
		//generate container/manager objects
		{
			world = new Engine::World();
			if (!world)
			{
				Shutdown();
				return false;
			}

			graphics = Lame::Graphics::Create(i_window);
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

		//create the player's object
		asteroid = std::shared_ptr<PlanetComponent>(PlanetComponent::Create(graphics->context(), world));
		if ( !asteroid || !graphics->Add(asteroid->renderable()) )
		{
			Shutdown();
			return false;
		}

		//Create our Materials
		//Create our Meshes
		//Create our renderables

		return true;
	}

	bool RunFrame()
	{
		eae6320::Time::OnNewFrame();
		const float deltaTime = eae6320::Time::GetSecondsElapsedThisFrame();

		HandleInput(deltaTime);

		//validate that all bullets, removing those that have expired
		const float bulletDuration = 2.5f;
		const float timeSinceStartup = eae6320::Time::GetTotalSecondsElapsed();
		for (std::vector<std::shared_ptr<BulletComponent>>::iterator itr = bullets.begin(); itr != bullets.end(); /*do not iterate here*/)
		{
			std::shared_ptr<BulletComponent> bullet = *itr;
			if (timeSinceStartup - bullet->get_creation_time() > bulletDuration)
			{
				itr = bullets.erase(itr);
				world->Remove(bullet->gameObject());
				graphics->Remove(bullet->renderable());
			}
			else
				itr++;
		}

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

		if (world)
		{
			delete world;
			world = nullptr;
		}
		if (graphics)
		{
			delete graphics;
			graphics = nullptr;
		}
		return true;
	}
}

namespace
{
	void HandleInput(float deltaTime)
	{
		using namespace System::UserInput;
		using namespace Engine;

		//fire bullet
		std::shared_ptr<GameObject> go = asteroid->gameObject();
		if (asteroid->can_fire() && Keyboard::Pressed(Keyboard::Space))
		{
			const Vector3 spawnPosition = go->position() + go->rotation().inverse() * Vector3(2.5f, 0.0f, 0.0f);
			std::shared_ptr<BulletComponent> bullet = CreateBullet();
			bullet->gameObject()->position(spawnPosition);
			bullet->gameObject()->rotation(go->rotation().inverse());
			asteroid->OnFire();
		}

		float movementAmount = 3.0f * deltaTime;
		std::shared_ptr<Engine::GameObject> movableObject = graphics->camera()->gameObject();
		if (Keyboard::Pressed(Keyboard::Up))						//forward
			movableObject->Move(Vector3(0.0f, 0.0f, -movementAmount));
		if (Keyboard::Pressed(Keyboard::Down))						//backward
			movableObject->Move(Vector3(0.0f, 0.0f, movementAmount));
		if (Keyboard::Pressed(Keyboard::Right))						//right
			movableObject->Move(Vector3(movementAmount, 0.0f, 0.0f));
		if (Keyboard::Pressed(Keyboard::Left))						//left
			movableObject->Move(Vector3(-movementAmount, 0.0f, 0.0f));
	}

	std::shared_ptr<BulletComponent> CreateBullet()
	{
		if (!graphics || !graphics->context())
			return nullptr;

		if (!bulletMat)
		{
			bulletMat = CreateMaterial("data/bullet.material.bin");
			if (!bulletMat)
				return nullptr;
		}

		if (!bulletMesh)
		{
			bulletMesh = CreateMesh("data/bullet.mesh.bin");
			if (!bulletMesh)
				return nullptr;
		}

		std::shared_ptr<Lame::RenderableComponent> renderable = CreateRenderableObject(bulletMesh, bulletMat);
		if (!renderable)
			return nullptr;

		const float bulletSpeed = 10.0f;
		const float creationTime = eae6320::Time::GetTotalSecondsElapsed();
		std::shared_ptr<BulletComponent> bullet = std::shared_ptr<BulletComponent>(new BulletComponent(creationTime, bulletSpeed, renderable));
		if (!bullet)
		{
			world->Remove(renderable->gameObject());
			graphics->Remove(renderable);
			return nullptr;
		}
		//bullet->gameObject()->enabled(false);
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
		std::shared_ptr<Engine::GameObject> go = world->AddNewGameObject();
		if (!go)
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
