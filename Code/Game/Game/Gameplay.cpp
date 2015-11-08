
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
	std::shared_ptr<Engine::GameObject> CreateObject(std::string i_mesh);

	std::unique_ptr<Lame::Graphics> graphics;

	//gameobject
	std::shared_ptr<Engine::GameObject> box;
	std::shared_ptr<Engine::GameObject> floorObject;

	//effect
	std::shared_ptr<Lame::Effect> effect;

	void HandleInput(float deltaTime);
}

namespace Gameplay
{
	bool Initialize(HWND i_window)
	{
		//generate a new graphics object
		{
			std::shared_ptr<Lame::Context> context(Lame::Context::Create(i_window));
			if (!context)	//if we failed to generate a context, shutdown the game
			{
				Shutdown();
				return false;
			}
			graphics = std::unique_ptr<Lame::Graphics>(Lame::Graphics::Create(context));
			if (!graphics) //if we failed to generate a graphics object, shutdown the game
			{
				Shutdown();
				return false;
			}
		}

		//initial camera position
		graphics->camera()->gameObject()->position(eae6320::Math::cVector(0, 0, 10));

		//load the effect we are going to use for everything in the game
		effect = std::shared_ptr<Lame::Effect>(Lame::Effect::Create(graphics->context(), "data/effect.effect.bin"));
		if (!effect)
			return nullptr;

		//Create our renderables
		box = CreateObject("data/box.mesh.bin");
		floorObject = CreateObject("data/floor.mesh.bin");
		if (!box || !floorObject)
		{
			Shutdown();
			return false;
		}
		floorObject->position(eae6320::Math::cVector(0.0f, -1.0f, 0.0f));
		
		std::string error;
		if (!eae6320::Time::Initialize(&error))
		{
			System::UserOutput::Display(error, "Time initialization error");
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
		box.reset();
		floorObject.reset();

		effect.reset();

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


		movableObject = box;
		if (KeyPressed('I') || KeyPressed('i'))						//up
			movableObject->Move(eae6320::Math::cVector(0.0f, movementAmount));
		if (KeyPressed('K') || KeyPressed('k'))						//down
			movableObject->Move(eae6320::Math::cVector(0.0f, -movementAmount));
		if (KeyPressed('L') || KeyPressed('l'))						//right
			movableObject->Move(eae6320::Math::cVector(movementAmount, 0.0f));
		if (KeyPressed('J') || KeyPressed('j'))						//left
			movableObject->Move(eae6320::Math::cVector(-movementAmount, 0.0f));
	}

	std::shared_ptr<Engine::GameObject> CreateObject(std::string i_mesh)
	{
		std::shared_ptr<Lame::Mesh> mesh(Lame::Mesh::Create(graphics->context(), i_mesh));
		if (!mesh)
			return nullptr;

		//create the gameObject
		std::shared_ptr<Engine::GameObject> go(new Engine::GameObject());
		if (!go)
			return nullptr;

		//create the renderable
		std::shared_ptr<Lame::RenderableComponent> renderable(new Lame::RenderableComponent(go, mesh, effect));
		if (!renderable)
			return nullptr;

		//add the renderable to the graphics system
		if (!graphics->Add(renderable))
			return nullptr;

		return go;
	}
}
