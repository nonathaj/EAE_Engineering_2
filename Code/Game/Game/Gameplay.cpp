
#include "Gameplay.h"

#include <vector>
#include <string>
#include <utility>

#include "../../Engine/LameEngine/Engine.h"

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
		if (!Lame::Engine::Setup(i_window))
			return false;

		using namespace Lame::Engine;

		Lame::Graphics* graphics = Lame::Engine::graphics();
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
		float deltaTime;
		if (!Lame::Engine::BeginFrame(deltaTime))
			return false;

		HandleInput(deltaTime);

#ifdef ENABLE_DEBUG_RENDERING
		Lame::Engine::graphics()->debug_renderer()->AddBox(
			true, 
			Lame::Vector3::one * 250.0f,
			Lame::Transform::CreateDefault());

		Lame::Engine::graphics()->debug_renderer()->AddLine(Lame::Vector3::zero, Lame::Vector3(0, 0, -1000), Lame::Color32::blue);
		Lame::Engine::graphics()->debug_renderer()->AddLine(Lame::Vector3(0, 100, 0), Lame::Vector3(0, 100, -1000), Lame::Color32::red);

#endif
		return Lame::Engine::EndFrame();
	}

	bool Shutdown()
	{
		sprite_effect.reset();
		sprite.reset();
		return Lame::Engine::Shutdown();
	}
}

namespace
{
	uint8_t GetNumberKeyPressed()
	{
		using namespace Lame::UserInput;
		if (Keyboard::Pressed(Keyboard::Alpha0))
			return 0;
		else if (Keyboard::Pressed(Keyboard::Alpha1))
			return 1;
		else if (Keyboard::Pressed(Keyboard::Alpha2))
			return 2;
		else if (Keyboard::Pressed(Keyboard::Alpha3))
			return 3;
		else if (Keyboard::Pressed(Keyboard::Alpha4))
			return 4;
		else if (Keyboard::Pressed(Keyboard::Alpha5))
			return 5;
		else if (Keyboard::Pressed(Keyboard::Alpha6))
			return 6;
		else if (Keyboard::Pressed(Keyboard::Alpha7))
			return 7;
		else if (Keyboard::Pressed(Keyboard::Alpha8))
			return 8;
		else if (Keyboard::Pressed(Keyboard::Alpha9))
			return 9;
		return -1;
	}

	void HandleInput(float deltaTime)
	{
		using namespace Lame;
		using namespace Lame::UserInput;

		Graphics* graphics = Lame::Engine::graphics();

		const float movementAmount = 300.0f * deltaTime;
		std::shared_ptr<GameObject> movableObject = graphics->camera()->gameObject();
		Vector3 movementVector = Vector3::zero;
		if (Keyboard::Pressed(Keyboard::W))						//forward
			movementVector += Vector3::forward;
		if (Keyboard::Pressed(Keyboard::S))						//backward
			movementVector += Vector3::back;
		if (Keyboard::Pressed(Keyboard::D))						//right
			movementVector += Vector3::right;
		if (Keyboard::Pressed(Keyboard::A))						//left
			movementVector += Vector3::left;
		if (Keyboard::Pressed(Keyboard::E))						//up
			movementVector += Vector3::up;
		if (Keyboard::Pressed(Keyboard::Q))						//down
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
		if (Keyboard::Pressed(Keyboard::Left))						//rotate left
			rotationAxis += Vector3::up;
		if (Keyboard::Pressed(Keyboard::Right))						//rotate Right
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
}
