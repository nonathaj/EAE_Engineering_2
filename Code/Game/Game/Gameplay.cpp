
#include "Gameplay.h"

#include <vector>
#include <string>

#include "../../Engine/Graphics/Context.h"
#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/Effect.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Core/Singleton.h"
#include "../../Engine/System/eae6320/Time.h"
#include "../../Engine/Component/GameObject.h"
#include "../../Engine/Graphics/RenderableComponent.h"
#include "../../Engine/System/UserInput.h"
#include "../../Engine/System/Console.h"
#include "../../Engine/System/UserOutput.h"

namespace
{
	std::shared_ptr<Lame::Material> CreateMaterial(std::string i_material);
	std::shared_ptr<Engine::GameObject> CreateObject(std::string i_mesh, std::shared_ptr<Lame::Material> i_material);

	std::unique_ptr<Lame::Graphics> graphics;

	//gameobject
	std::shared_ptr<Engine::GameObject> transparent_box_foreground;
	std::shared_ptr<Engine::GameObject> transparent_box_main;
	std::shared_ptr<Engine::GameObject> box_background;
	std::shared_ptr<Engine::GameObject> box_foreground;
	std::shared_ptr<Engine::GameObject> movable;
	std::shared_ptr<Engine::GameObject> floorObject;

	//effects
	std::shared_ptr<Lame::Material> green_transparent;
	std::shared_ptr<Lame::Material> blue_transparent;
	std::shared_ptr<Lame::Material> red_opaque;
	std::shared_ptr<Lame::Material> green_opaque;

	void HandleInput(float deltaTime);
}

namespace Gameplay
{
	bool Initialize(HWND i_window)
	{
		using namespace Lame;

		//generate a new graphics object
		{
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
		}

		//initial camera position
		graphics->camera()->gameObject()->position(eae6320::Math::cVector(0, 0, 5));

		//load the effect we are going to use for everything in the game
		green_transparent = CreateMaterial("data/green_transparent.material.bin");
		blue_transparent = CreateMaterial("data/blue_transparent.material.bin");
		red_opaque = CreateMaterial("data/red_opaque.material.bin");
		green_opaque = CreateMaterial("data/green_opaque.material.bin");
		if (!green_transparent || !blue_transparent || !red_opaque || !green_opaque)
		{
			Shutdown();
			return false;
		}

		//Create our renderables
		transparent_box_foreground = CreateObject("data/box.mesh.bin", green_transparent);
		transparent_box_main = CreateObject("data/box.mesh.bin", blue_transparent);
		box_foreground = CreateObject("data/box.mesh.bin", red_opaque);
		box_background = CreateObject("data/box.mesh.bin", green_opaque);

		movable = CreateObject("data/white_triangle_prism.mesh.bin", green_opaque);
		floorObject = CreateObject("data/white_floor.mesh.bin", red_opaque);

		if (!transparent_box_foreground || !transparent_box_main || 
			!box_foreground  || !box_background|| !movable || !floorObject)
		{
			Shutdown();
			return false;
		}
		floorObject->position(eae6320::Math::cVector(0.0f, -0.5f, 0.0f));
		transparent_box_foreground->position(eae6320::Math::cVector(1.5f, 0.0f, 0.0f));
		box_foreground->position(eae6320::Math::cVector(1.5f, -0.5f, -1.5f));
		box_background->position(eae6320::Math::cVector(1.5f, 0.5f, 1.5f));
		transparent_box_main->position(eae6320::Math::cVector(-1.5f, 0.0f, -1.5f));
		
		std::string error;
		if (!eae6320::Time::Initialize(&error))
		{
			System::UserOutput::Display(error, "Time initialization error");
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

		bool renderSuccess = graphics->Render();
		return renderSuccess;
	}

	bool Shutdown()
	{
		transparent_box_foreground.reset();
		transparent_box_main.reset();
		box_background.reset();
		box_foreground.reset();
		movable.reset();
		floorObject.reset();

		green_transparent.reset();
		blue_transparent.reset();
		red_opaque.reset();
		green_opaque.reset();

		graphics.reset();
		return true;
	}
}

namespace
{
	void HandleInput(float deltaTime)
	{
		using namespace System::UserInput;

		float movementAmount = 3.0f * deltaTime;

		std::shared_ptr<Engine::GameObject> movableObject = graphics->camera()->gameObject();
		if (KeyPressed('W') || KeyPressed('w'))						//forward
			movableObject->Move(eae6320::Math::cVector(0.0f, 0.0f, -movementAmount));
		if(KeyPressed('S') || KeyPressed('s'))						//backward
			movableObject->Move(eae6320::Math::cVector(0.0f, 0.0f, movementAmount));
		if (KeyPressed('D') || KeyPressed('d'))						//right
			movableObject->Move(eae6320::Math::cVector(movementAmount, 0.0f, 0.0f));
		if (KeyPressed('A') || KeyPressed('a'))						//left
			movableObject->Move(eae6320::Math::cVector(-movementAmount, 0.0f, 0.0f));


		movableObject = movable;
		if (KeyPressed('I') || KeyPressed('i'))						//up
			movableObject->Move(eae6320::Math::cVector(0.0f, movementAmount));
		if (KeyPressed('K') || KeyPressed('k'))						//down
			movableObject->Move(eae6320::Math::cVector(0.0f, -movementAmount));
		if (KeyPressed('L') || KeyPressed('l'))						//right
			movableObject->Move(eae6320::Math::cVector(movementAmount, 0.0f));
		if (KeyPressed('J') || KeyPressed('j'))						//left
			movableObject->Move(eae6320::Math::cVector(-movementAmount, 0.0f));
	}

	std::shared_ptr<Lame::Material> CreateMaterial(std::string i_material)
	{
		if (!graphics || !graphics->context())
			return nullptr;

		using namespace Lame;
		return std::shared_ptr<Material>(Material::Create(graphics->context(), i_material));

	}

	std::shared_ptr<Engine::GameObject> CreateObject(std::string i_mesh, std::shared_ptr<Lame::Material> i_material)
	{
		if (!graphics || !graphics->context())
			return nullptr;

		using namespace Lame;

		std::shared_ptr<Mesh> mesh(Mesh::Create(graphics->context(), i_mesh));
		if (!mesh)
			return nullptr;

		//create the gameObject
		std::shared_ptr<Engine::GameObject> go(new Engine::GameObject());
		if (!go)
			return nullptr;

		//create the renderable
		std::shared_ptr<RenderableComponent> renderable(new RenderableComponent(go, mesh, i_material));
		if (!renderable)
			return nullptr;

		//add the renderable to the graphics system
		if (!graphics->Add(renderable))
			return nullptr;

		return go;
	}
}
