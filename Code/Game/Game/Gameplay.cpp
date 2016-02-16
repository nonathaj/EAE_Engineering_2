
#include "Gameplay.h"

#include <vector>
#include <string>
#include <utility>

#include "../../Engine/Graphics/Context.h"
#include "../../Engine/Graphics/RenderableMesh.h"
#include "../../Engine/Graphics/Effect.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Graphics/Sprite.h"
#include "../../Engine/Graphics/Texture.h"
#include "../../Engine/Core/Color.h"
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
#include "../../Engine/Physics/Physics.h"

namespace
{
	std::shared_ptr<Lame::Material> CreateMaterial(const std::string& i_material);
	std::shared_ptr<Lame::RenderableMesh> CreateRenderableMesh(const std::string& i_mesh);
	std::shared_ptr<Lame::RenderableComponent> CreateRenderableObject(std::shared_ptr<Lame::GameObject> i_go, std::shared_ptr<Lame::RenderableMesh> i_mesh, std::shared_ptr<Lame::Material> i_material);

	std::shared_ptr<Lame::Effect> sprite_effect;

	std::shared_ptr<Lame::Sprite> sprite;
	std::shared_ptr<Lame::Sprite> number;

	void HandleInput(float deltaTime);

	bool Contacting(std::shared_ptr<Lame::GameObject> go1, std::shared_ptr<Lame::GameObject> go2, const float& go1Size, const float& go2Size);
	uint8_t GetNumberKeyPressed();

	float framerate = 0.0f;
	bool red_sphere = false;
	bool show_sphere = true;
	float sphere_radius = 250.0f;
	char frames_per_second[50];
}

namespace Gameplay
{
	bool Initialize(HWND i_window)
	{
		{
			if (!LameWorld::Get().Setup() || 
				!LameGraphics::Get().Setup(i_window) ||
				!LamePhysics::Get().Setup())
			{
				Shutdown();
				return false;
			}

#ifdef ENABLE_DEBUG_RENDERING
			//enable debug drawing for graphics
			if (!LameGraphics::Get().EnableDebugDrawing(10000))
			{
				Shutdown();
				return false;
			}
#endif

#ifdef ENABLE_DEBUG_MENU
			LameGraphics::Get().debug_menu()->CreateText("FPS", frames_per_second);
			LameGraphics::Get().debug_menu()->CreateCheckBox("Color Sphere Red ", &red_sphere);
			LameGraphics::Get().debug_menu()->CreateSlider("Sphere Radius", &sphere_radius, 100.0f, 500.0f);
			LameGraphics::Get().debug_menu()->CreateButton("Reset Sphere Radius", []() { sphere_radius = 250.0f; });
			LameGraphics::Get().debug_menu()->CreateCheckBox("Show Sphere", &show_sphere);
#endif

			std::string error;
			if (!eae6320::Time::Initialize(&error))
			{
				Lame::UserOutput::Display(error, "Time initialization error");
				Shutdown();
				return false;
			}
		}

		LameGraphics::Get().camera()->gameObject()->transform().position(Lame::Vector3(0, 0, 800.0f));
		LameGraphics::Get().camera()->near_clip_plane(1.0f);
		LameGraphics::Get().camera()->far_clip_plane(5000.0f);

		if (
			!CreateRenderableObject(nullptr, CreateRenderableMesh("data/ceiling_mesh.mesh.bin"), CreateMaterial("data/cement_wall.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateRenderableMesh("data/cement_mesh.mesh.bin"), CreateMaterial("data/cement_wall.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateRenderableMesh("data/floor_mesh.mesh.bin"), CreateMaterial("data/floor.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateRenderableMesh("data/metal_mesh.mesh.bin"), CreateMaterial("data/metal_brace.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateRenderableMesh("data/railing_mesh.mesh.bin"), CreateMaterial("data/railing.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateRenderableMesh("data/walls_mesh.mesh.bin"), CreateMaterial("data/wall.material.bin")) ||
			!CreateRenderableObject(nullptr, CreateRenderableMesh("data/lambert_objects_mesh.mesh.bin"), CreateMaterial("data/white.material.bin"))
			)
		{
			Shutdown();
			return false;
		}

		sprite_effect = std::shared_ptr<Lame::Effect>(Lame::Effect::Create(LameGraphics::Get().context(), "data/sprite.effect.bin"));
		if (!sprite_effect)
		{
			Shutdown();
			return false;
		}

		sprite = std::shared_ptr<Lame::Sprite>(
			Lame::Sprite::Create(
			sprite_effect,
			std::shared_ptr<Lame::Texture>(Lame::Texture::Create(LameGraphics::Get().context(), "data/alpha.DDS")),
			Lame::Vector2::one * 0.1f,
			0.2f,
			Lame::Rectangle2D::CreateTLNormalized() ));
		if (!sprite || !LameGraphics::Get().Add(sprite))
		{
			Shutdown();
			return false;
		}

		number = std::shared_ptr<Lame::Sprite>(Lame::Sprite::Create(
			sprite_effect,
			std::shared_ptr<Lame::Texture>(Lame::Texture::Create(LameGraphics::Get().context(), "data/numbers.DDS")),
			Lame::Rectangle2D(Lame::Vector2::one * 0.8f, Lame::Vector2(0.2f, 0.3f)),
			Lame::Rectangle2D::CreateTLNormalized() ));
		if (!number || !LameGraphics::Get().Add(number))
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
		_itoa_s(static_cast<int>(1.0f / deltaTime), frames_per_second, 10);
		
		HandleInput(deltaTime);

#ifdef ENABLE_DEBUG_RENDERING
		if (show_sphere)
		{
			LameGraphics::Get().debug_renderer()->AddSphere(
				true,
				1.0f,
				Lame::Transform(Lame::Vector3::zero, Lame::Quaternion::identity, Lame::Vector3::one * sphere_radius),
				red_sphere ? Lame::Color32::red : Lame::Color32::white);
		}
#endif

		LamePhysics::Get().Tick(deltaTime);
		LameWorld::Get().Update(deltaTime);
		return LameGraphics::Get().Render();
	}

	bool Shutdown()
	{
		sprite_effect.reset();
		sprite.reset();

		LamePhysics::Release();
		LameGraphics::Release();
		LameWorld::Release();
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
		std::shared_ptr<GameObject> movableObject = LameGraphics::Get().camera()->gameObject();
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

		movementVector = LameGraphics::Get().camera()->gameObject()->transform().rotation() * movementVector * movementAmount;
		if (!Math::Float::IsNaN(movementVector.x()) &&
			!Math::Float::IsNaN(movementVector.y()) &&
			!Math::Float::IsNaN(movementVector.z()))
		{
			LameGraphics::Get().camera()->gameObject()->transform().Move(movementVector);
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
		if (!LameGraphics::Exists())
			return nullptr;
		return std::shared_ptr<Material>(Material::Create(LameGraphics::Get().context(), i_material));
	}

	std::shared_ptr<Lame::RenderableMesh> CreateRenderableMesh(const std::string& i_mesh)
	{
		using namespace Lame;
		if (!LameGraphics::Exists())
			return nullptr;
		return std::shared_ptr<RenderableMesh>(RenderableMesh::Create(true, LameGraphics::Get().context(), i_mesh));
	}

	std::shared_ptr<Lame::RenderableComponent> CreateRenderableObject(std::shared_ptr<Lame::GameObject> i_go, std::shared_ptr<Lame::RenderableMesh> i_mesh, std::shared_ptr<Lame::Material> i_material)
	{
		using namespace Lame;

		if (!LameWorld::Exists() || !LameGraphics::Exists() || !i_mesh || !i_material)
			return nullptr;

		//create the gameObject, if we did not receive a valid one
		std::shared_ptr<GameObject> go = i_go;
		if (!go)
		{
			go = LameWorld::Get().AddNewGameObject();
			if (!go)
				return nullptr;
		}
		else
		{
			if (!LameWorld::Get().Add(go))
				return nullptr;
		}

		//create the renderable
		std::shared_ptr<RenderableComponent> renderable(RenderableComponent::Create(go, i_mesh, i_material));
		if (!renderable)
		{
			LameWorld::Get().Remove(go);
			return nullptr;
		}

		//add the renderable to the graphics system
		if (!LameGraphics::Get().Add(renderable))
		{
			LameWorld::Get().Remove(go);
			return nullptr;
		}

		return renderable;
	}
}
