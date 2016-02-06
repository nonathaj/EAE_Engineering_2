
#include "Gameplay.h"

#include <vector>
#include <string>
#include <utility>

#include "../../Engine/Graphics/Context.h"
#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/Effect.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Graphics/Sprite.h"
#include "../../Engine/Graphics/Texture.h"
#include "../../Engine/Core/Singleton.h"
#include "../../Engine/System/eae6320/Time.h"
#include "../../Engine/Component/World.h"
#include "../../Engine/Component/Transform.h"
#include "../../Engine/Component/GameObject.h"
#include "../../Engine/Graphics/RenderableComponent.h"
#include "../../Engine/System/UserInput.h"
#include "../../Engine/System/Console.h"
#include "../../Engine/System/UserOutput.h"
#include "../../Engine/Core/Math.h"
#include "../../Engine/Core/Vector3.h"
#include "../../Engine/Core/Random.h"

namespace
{
	std::shared_ptr<Lame::Material> CreateMaterial(const std::string& i_material);
	std::shared_ptr<Lame::Mesh> CreateMesh(const std::string& i_mesh);
	std::shared_ptr<Lame::RenderableComponent> CreateRenderableObject(std::shared_ptr<Lame::GameObject> i_go, std::shared_ptr<Lame::Mesh> i_mesh, std::shared_ptr<Lame::Material> i_material);

	std::unique_ptr<Lame::Graphics> graphics;
	std::unique_ptr<Lame::World> world;

	std::shared_ptr<Lame::Effect> sprite_effect;

	std::shared_ptr<Lame::Sprite> sprite;
	std::shared_ptr<Lame::Sprite> number;

	void HandleInput(float deltaTime);

	bool Contacting(std::shared_ptr<Lame::GameObject> go1, std::shared_ptr<Lame::GameObject> go2, const float& go1Size, const float& go2Size);
	uint8_t GetNumberKeyPressed();
}

namespace Gameplay
{
	bool Initialize(HWND i_window)
	{
		{
			world = std::unique_ptr<Lame::World>(new Lame::World());
			if (!world)
			{
				Shutdown();
				return false;
			}

			graphics = std::unique_ptr<Lame::Graphics>(Lame::Graphics::Create(i_window));
			if (!graphics)
			{
				Shutdown();
				return false;
			}

#ifdef ENABLE_DEBUG_RENDERING
			//enable debug drawing for graphics
			if (!graphics->EnableDebugDrawing(10000))
			{
				Shutdown();
				return false;
			}
#endif

			std::string error;
			if (!eae6320::Time::Initialize(&error))
			{
				Lame::UserOutput::Display(error, "Time initialization error");
				Shutdown();
				return false;
			}
		}

		graphics->camera()->gameObject()->transform().position(Lame::Vector3(0, 0, 15));
		graphics->camera()->near_clip_plane(1.0f);
		graphics->camera()->far_clip_plane(5000.0f);

		if (
			!CreateRenderableObject(nullptr, CreateMesh("data/ceiling_mesh.mesh.bin"), CreateMaterial("data/cement_wall.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateMesh("data/cement_mesh.mesh.bin"), CreateMaterial("data/cement_wall.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateMesh("data/floor_mesh.mesh.bin"), CreateMaterial("data/floor.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateMesh("data/metal_mesh.mesh.bin"), CreateMaterial("data/metal_brace.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateMesh("data/railing_mesh.mesh.bin"), CreateMaterial("data/railing.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateMesh("data/walls_mesh.mesh.bin"), CreateMaterial("data/wall.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateMesh("data/lambert_objects_mesh.mesh.bin"), CreateMaterial("data/white.material.bin"))
			)
		{
			Shutdown();
			return false;
		}

		sprite_effect = std::shared_ptr<Lame::Effect>(Lame::Effect::Create(graphics->context(), "data/sprite.effect.bin"));
		if (!sprite_effect)
		{
			Shutdown();
			return false;
		}

		sprite = std::shared_ptr<Lame::Sprite>(
			Lame::Sprite::Create(
			sprite_effect,
			std::shared_ptr<Lame::Texture>(Lame::Texture::Create(graphics->context(), "data/alpha.DDS")),
			Lame::Vector2::one * 0.1f,
			0.2f,
			Lame::Rectangle2D::CreateTLNormalized() ));
		if (!sprite || !graphics->Add(sprite))
		{
			Shutdown();
			return false;
		}

		number = std::shared_ptr<Lame::Sprite>(Lame::Sprite::Create(
			sprite_effect,
			std::shared_ptr<Lame::Texture>(Lame::Texture::Create(graphics->context(), "data/numbers.DDS")),
			Lame::Rectangle2D(Lame::Vector2::one * 0.8f, Lame::Vector2(0.2f, 0.3f)),
			Lame::Rectangle2D::CreateTLNormalized() ));
		if (!number || !graphics->Add(number))
		{
			Shutdown();
			return false;
		}
		number->SelectFromSheet(10, 1, 0);

		return true;
	}

	bool RunFrame()
	{
		eae6320::Time::OnNewFrame();
		float deltaTime = eae6320::Time::GetSecondsElapsedThisFrame();
		LameInput::Get().Tick(deltaTime);

		HandleInput(deltaTime);

#ifdef ENABLE_DEBUG_RENDERING
		graphics->debug_renderer()->AddBox(
			true, 
			Lame::Vector3::one * 250.0f,
			Lame::Transform::CreateDefault() );
		graphics->debug_renderer()->AddLine(Lame::Vector3::zero, Lame::Vector3(0, 0, -1000), Lame::Color32::blue);
		graphics->debug_renderer()->AddLine(Lame::Vector3(0, 100, 0), Lame::Vector3(0, 100, -1000), Lame::Color32::red);
#endif

		world->Update(deltaTime);
		return graphics->Render();
	}

	bool Shutdown()
	{
		sprite_effect.reset();
		sprite.reset();
		graphics.reset();
		world.reset();
		LameInput::Release();
		return true;
	}
}

namespace
{
	uint8_t GetNumberKeyPressed()
	{
		using namespace Lame::Input;
		if (LameInput::Get().Down(Keyboard::Alpha0))
			return 0;
		else if (LameInput::Get().Down(Keyboard::Alpha1))
			return 1;
		else if (LameInput::Get().Down(Keyboard::Alpha2))
			return 2;
		else if (LameInput::Get().Down(Keyboard::Alpha3))
			return 3;
		else if (LameInput::Get().Down(Keyboard::Alpha4))
			return 4;
		else if (LameInput::Get().Down(Keyboard::Alpha5))
			return 5;
		else if (LameInput::Get().Down(Keyboard::Alpha6))
			return 6;
		else if (LameInput::Get().Down(Keyboard::Alpha7))
			return 7;
		else if (LameInput::Get().Down(Keyboard::Alpha8))
			return 8;
		else if (LameInput::Get().Down(Keyboard::Alpha9))
			return 9;
		return -1;
	}

	void HandleInput(float deltaTime)
	{
		using namespace Lame;
		using namespace Lame::Input;

		const float movementAmount = 300.0f * deltaTime;
		std::shared_ptr<GameObject> movableObject = graphics->camera()->gameObject();
		Vector3 movementVector = Vector3::zero;
		if (LameInput::Get().Held(Keyboard::W))						//forward
			movementVector += Vector3::forward;
		if (LameInput::Get().Held(Keyboard::S))						//backward
			movementVector += Vector3::back;
		if (LameInput::Get().Held(Keyboard::D))						//right
			movementVector += Vector3::right;
		if (LameInput::Get().Held(Keyboard::A))						//left
			movementVector += Vector3::left;
		if (LameInput::Get().Held(Keyboard::E))						//up
			movementVector += Vector3::up;
		if (LameInput::Get().Held(Keyboard::Q))						//down
			movementVector += Vector3::down;

		movementVector = graphics->camera()->gameObject()->transform().rotation() * movementVector * movementAmount;
		if (!Math::Float::IsNaN(movementVector.x()) &&
			!Math::Float::IsNaN(movementVector.y()) &&
			!Math::Float::IsNaN(movementVector.z()))
		{
			graphics->camera()->gameObject()->transform().Move(movementVector);
		}
		else
		{
			DEBUG_PRINT("Invalid movement vector");
		}

		const float rotationAmount = 40.0f * deltaTime;
		Vector3 rotationAxis = Vector3::zero;
		if (LameInput::Get().Held(Keyboard::Left))						//rotate left
			rotationAxis += Vector3::up;
		if (LameInput::Get().Held(Keyboard::Right))						//rotate Right
			rotationAxis += Vector3::down;
		//if (Keyboard::Pressed(Keyboard::Up))						//rotate up
		//	rotationAxis += Vector3::Vector3::right;
		//if (Keyboard::Pressed(Keyboard::Down))						//rotate Down
		//	rotationAxis += Vector3::Vector3::left;

		movableObject->transform().Rotate(Quaternion::Euler(rotationAxis * rotationAmount));

		uint8_t num = GetNumberKeyPressed();
		if (num < 10)
		{
			number->SelectFromSheet(10, 1, num);
		}
	}

	bool Contacting(std::shared_ptr<Lame::GameObject> go1, std::shared_ptr<Lame::GameObject> go2, const float& go1Size, const float& go2Size)
	{
		return go1->transform().position().distance(go2->transform().position()) <= go1Size + go2Size;
	}

	std::shared_ptr<Lame::Material> CreateMaterial(const std::string& i_material)
	{
		using namespace Lame;

		if (!graphics || !graphics->context())
			return nullptr;
		return std::shared_ptr<Material>(Material::Create(graphics->context(), i_material));
	}

	std::shared_ptr<Lame::Mesh> CreateMesh(const std::string& i_mesh)
	{
		using namespace Lame;

		if (!graphics || !graphics->context())
			return nullptr;
		return std::shared_ptr<Mesh>(Mesh::Create(graphics->context(), i_mesh));
	}

	std::shared_ptr<Lame::RenderableComponent> CreateRenderableObject(std::shared_ptr<Lame::GameObject> i_go, std::shared_ptr<Lame::Mesh> i_mesh, std::shared_ptr<Lame::Material> i_material)
	{
		using namespace Lame;

		if (!world || !graphics || !graphics->context() || !i_mesh || !i_material)
			return nullptr;

		//create the gameObject, if we did not receive a valid one
		std::shared_ptr<GameObject> go = i_go;
		if (!go)
		{
			go = world->AddNewGameObject();
			if (!go)
				return nullptr;
		}
		else
		{
			if (!world->Add(go))
				return nullptr;
		}

		//create the renderable
		std::shared_ptr<RenderableComponent> renderable(RenderableComponent::Create(go, i_mesh, i_material));
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
