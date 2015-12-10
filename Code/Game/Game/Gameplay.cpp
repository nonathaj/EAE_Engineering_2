
#include "Gameplay.h"

#include <vector>
#include <string>
#include <utility>

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
#include "../../Engine/Core/Random.h"

#include "BulletComponent.h"
#include "PlanetComponent.h"
#include "EnemyComponent.h"

namespace
{
	std::shared_ptr<Lame::Material> CreateMaterial(std::string i_material);
	std::shared_ptr<Lame::Mesh> CreateMesh(std::string i_mesh);
	std::shared_ptr<Lame::RenderableComponent> CreateRenderableObject(std::shared_ptr<Lame::Mesh> i_mesh, std::shared_ptr<Lame::Material> i_material);

	std::shared_ptr<BulletComponent> CreateBullet();
	std::shared_ptr<EnemyComponent> CreateEnemy();

	Lame::Graphics *graphics = nullptr;
	Engine::World *world = nullptr;

	std::shared_ptr<PlanetComponent> asteroid;

	std::vector<std::shared_ptr<BulletComponent>> bullets;
	std::vector<std::shared_ptr<EnemyComponent>> enemies;

	std::shared_ptr<Lame::Material> bulletMat;
	std::shared_ptr<Lame::Mesh> bulletMesh;

	std::pair<std::shared_ptr<Lame::Mesh>, std::shared_ptr<Lame::Material>> pistachio;
	std::pair<std::shared_ptr<Lame::Mesh>, std::shared_ptr<Lame::Material>> cashew;

	const float asteroidSize = 2;
	const float bulletSize = 1.0f / 2;
	const float enemySize = 1.0f;

	const float enemyCreationDelay = 0.75f;
	float enemyCreationTimer = 0.0f;

	const float enemySpawnDistance = 20.0f;

	void HandleInput(float deltaTime);

	bool Contacting(std::shared_ptr<Engine::GameObject> go1, std::shared_ptr<Engine::GameObject> go2, const float& go1Size, const float& go2Size);
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
		asteroid->set_weapon_cooldown(0.25f);
		asteroid->set_rotation_degrees_per_second(120.0f);

		{
			pistachio.first = CreateMesh("data/pistachio.mesh.bin");
			pistachio.second = CreateMaterial("data/pistachio.material.bin");

			cashew.first = CreateMesh("data/cashew.mesh.bin");			
			cashew.second = CreateMaterial("data/cashew.material.bin");
			if (!pistachio.first || !pistachio.second || !cashew.first || !cashew.second)
			{
				Shutdown();
				return false;
			}
		}

		return true;
	}

	bool RunFrame()
	{
		eae6320::Time::OnNewFrame();
		const float deltaTime = eae6320::Time::GetSecondsElapsedThisFrame();

		HandleInput(deltaTime);

		//validate all bullets, removing those that have expired
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
				++itr;
		}

		//validate all enemies, removing those that are close to the planet or any bullets
		for (auto itr = enemies.begin(); itr != enemies.end(); /*  */)
		{
			std::shared_ptr<EnemyComponent> enemy = *itr;
			std::shared_ptr<Engine::GameObject> go = enemy->gameObject();

			bool shouldDelete = false;

			//if we are hitting the planet, destroy this enemy
			if (Contacting(go, asteroid->gameObject(), enemySize, asteroidSize))
				shouldDelete = true;
			else
			{
				//if we are contacting any bullet, destroy the bullet and this enemy
				for (std::vector<std::shared_ptr<BulletComponent>>::iterator itr = bullets.begin(); itr != bullets.end(); /*do not iterate here*/)
				{
					std::shared_ptr<BulletComponent> bullet = *itr;
					if (Contacting(go, bullet->gameObject(), enemySize, bulletSize))
					{
						itr = bullets.erase(itr);
						world->Remove(bullet->gameObject());
						graphics->Remove(bullet->renderable());
						shouldDelete = true;
						break;
					}
					else
						++itr;
				}
			}

			if (shouldDelete)
			{
				itr = enemies.erase(itr);
				world->Remove(go);
				graphics->Remove(enemy->renderable());
			}
			else
			{
				++itr;
			}
		}

		//create a new enemy on a timer
		enemyCreationTimer -= deltaTime;
		if (enemyCreationTimer < 0.0f)
		{
			enemyCreationTimer = enemyCreationDelay;
			std::shared_ptr<EnemyComponent> enemy = CreateEnemy();
			Engine::Vector3 spawnLocation(enemySpawnDistance, 0.0f, 0.0f);
			spawnLocation = Engine::Quaternion(Engine::Random::Range(0.0f, 2.0f * static_cast<float>(M_PI)), Engine::Vector3(0.0f, 0.0f, 1.0f)) * spawnLocation;
			enemy->gameObject()->position(spawnLocation);
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

		pistachio.first.reset();
		pistachio.second.reset();
		cashew.first.reset();
		cashew.second.reset();
		enemies.clear();

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

	bool Contacting(std::shared_ptr<Engine::GameObject> go1, std::shared_ptr<Engine::GameObject> go2, const float& go1Size, const float& go2Size)
	{
		return go1->position().distance(go2->position()) <= go1Size + go2Size;
	}

	std::shared_ptr<BulletComponent> CreateBullet()
	{
		if (!world || !graphics || !graphics->context())
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
		bullets.push_back(bullet);
		return bullet;
	}

	std::shared_ptr<EnemyComponent> CreateEnemy()
	{
		if (!pistachio.first || !pistachio.second || !cashew.first || !cashew.second)
			return nullptr;

		std::shared_ptr<Lame::RenderableComponent> renderable;
		bool isCashew = Engine::Random::Value() > 0.5f;
		if (isCashew)
			renderable = CreateRenderableObject(cashew.first, cashew.second);
		else
			renderable = CreateRenderableObject(pistachio.first, pistachio.second);

		if (!renderable)
			return nullptr;

		std::shared_ptr<EnemyComponent> enemy(new EnemyComponent(renderable));
		if (!enemy)
		{
			world->Remove(renderable->gameObject());
			graphics->Remove(renderable);
			return nullptr;
		}

		enemies.push_back(enemy);
		return enemy;
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
