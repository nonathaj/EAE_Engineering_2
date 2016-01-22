
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

	Lame::Graphics *graphics = nullptr;
	Engine::World *world = nullptr;

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
		graphics->camera()->far_clip_plane(15000.0f);

		if (!CreateRenderableObject(CreateMesh("data/ceiling_mesh.mesh.bin"), CreateMaterial("data/cement_wall.material.bin")) ||
			!CreateRenderableObject(CreateMesh("data/cement_mesh.mesh.bin"), CreateMaterial("data/cement_wall.material.bin")) ||
			!CreateRenderableObject(CreateMesh("data/floor_mesh.mesh.bin"), CreateMaterial("data/floor.material.bin")) || 
			!CreateRenderableObject(CreateMesh("data/metal_mesh.mesh.bin"), CreateMaterial("data/metal_brace.material.bin")) || 
			!CreateRenderableObject(CreateMesh("data/railing_mesh.mesh.bin"), CreateMaterial("data/railing.material.bin")) || 
			!CreateRenderableObject(CreateMesh("data/walls_mesh.mesh.bin"), CreateMaterial("data/wall.material.bin")))
		{
			Shutdown();
			return false;
		}

		return true;
	}

	bool RunFrame()
	{
		eae6320::Time::OnNewFrame();
		const float deltaTime = eae6320::Time::GetSecondsElapsedThisFrame();

		HandleInput(deltaTime);

		world->Update(deltaTime);
		bool renderSuccess = graphics->Render();
		return renderSuccess;
	}

	bool Shutdown()
	{
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

		const float movementAmount = 300.0f * deltaTime;
		std::shared_ptr<Engine::GameObject> movableObject = graphics->camera()->gameObject();
		Vector3 movementVector = Vector3::zero;
		if (Keyboard::Pressed(Keyboard::W))						//forward
			movementVector += Vector3::forward * movementAmount;
		if (Keyboard::Pressed(Keyboard::S))						//backward
			movementVector += Vector3::back * movementAmount;
		if (Keyboard::Pressed(Keyboard::D))						//right
			movementVector += Vector3::right * movementAmount;
		if (Keyboard::Pressed(Keyboard::A))						//left
			movementVector += Vector3::left * movementAmount;
		if (Keyboard::Pressed(Keyboard::E))						//up
			movementVector += Vector3::up * movementAmount;
		if (Keyboard::Pressed(Keyboard::Q))						//down
			movementVector += Vector3::down * movementAmount;

		movementVector = graphics->camera()->gameObject()->rotation() * movementVector;
		graphics->camera()->gameObject()->Move(movementVector);

		const float rotationAmount = 30.0f * deltaTime;
		if (Keyboard::Pressed(Keyboard::Left))						//rotate left
			movableObject->Rotate(Quaternion::Euler(0.0f, -movementAmount, 0.0f));
		if (Keyboard::Pressed(Keyboard::Right))						//rotate Right
			movableObject->Rotate(Quaternion::Euler(0.0f, movementAmount, 0.0f));
		//if (Keyboard::Pressed(Keyboard::Up))						//rotate up
		//	movableObject->Rotate(Quaternion::Euler(movementAmount, 0.0f, 0.0f));
		//if (Keyboard::Pressed(Keyboard::Down))						//rotate Down
		//	movableObject->Rotate(Quaternion::Euler(-movementAmount, 0.0f, 0.0f));
	}

	bool Contacting(std::shared_ptr<Engine::GameObject> go1, std::shared_ptr<Engine::GameObject> go2, const float& go1Size, const float& go2Size)
	{
		return go1->position().distance(go2->position()) <= go1Size + go2Size;
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
