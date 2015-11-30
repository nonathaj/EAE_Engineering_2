
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
	std::shared_ptr<Engine::GameObject> movable;
	std::shared_ptr<Engine::GameObject> floorObject;

	std::shared_ptr<Engine::GameObject> logo1;
	std::shared_ptr<Engine::GameObject> logo2;
	std::shared_ptr<Engine::GameObject> logo3;
	std::shared_ptr<Engine::GameObject> logo4;
	std::shared_ptr<Engine::GameObject> logo5;
	std::shared_ptr<Engine::GameObject> box1;
	std::shared_ptr<Engine::GameObject> box2;
	std::shared_ptr<Engine::GameObject> box3;
	std::shared_ptr<Engine::GameObject> box4;
	std::shared_ptr<Engine::GameObject> box5;

	//effects
	std::shared_ptr<Lame::Material> green_transparent;
	std::shared_ptr<Lame::Material> red_opaque;
	std::shared_ptr<Lame::Material> green_opaque;

	std::shared_ptr<Lame::Material> eae6320Mat;
	std::shared_ptr<Lame::Material> alphaMat;
	std::shared_ptr<Lame::Material> notalphaMat;

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
		red_opaque = CreateMaterial("data/red_opaque.material.bin");
		green_opaque = CreateMaterial("data/green_opaque.material.bin");
		eae6320Mat = CreateMaterial("data/eae6320.material.bin");
		alphaMat = CreateMaterial("data/alpha.material.bin");
		notalphaMat = CreateMaterial("data/notalpha.material.bin");
		if (!green_transparent || !red_opaque || !green_opaque || !eae6320Mat || !alphaMat || !notalphaMat)
		{
			Shutdown();
			return false;
		}

		//Create our renderables
		logo1 = CreateObject("data/square.mesh.bin", green_transparent);
		box1 = CreateObject("data/box.mesh.bin", green_transparent);

		logo2 = CreateObject("data/square.mesh.bin", green_opaque);
		box2 = CreateObject("data/box.mesh.bin", green_opaque);

		logo3 = CreateObject("data/square.mesh.bin", eae6320Mat);
		box3 = CreateObject("data/box.mesh.bin", eae6320Mat);

		logo4 = CreateObject("data/square.mesh.bin", notalphaMat);
		box4 = CreateObject("data/box.mesh.bin", notalphaMat);

		logo5 = CreateObject("data/square.mesh.bin", eae6320Mat);
		box5 = CreateObject("data/box.mesh.bin", eae6320Mat);

		movable = CreateObject("data/white_triangle_prism.mesh.bin", green_opaque);
		floorObject = CreateObject("data/white_floor.mesh.bin", red_opaque);

		if (!movable || !floorObject ||
			!logo1 || !logo2 || !logo3 || !logo4 || !logo5 ||
			!box1 || !box2 || !box3 || !box4 || !box5 )
		{
			Shutdown();
			return false;
		}
		floorObject->position(eae6320::Math::cVector(0.0f, -1.0f, 0.0f));

		logo1->position(eae6320::Math::cVector(-2.0f, 0.5f, 1.5f));
		box1->position(eae6320::Math::cVector(-2.0f, -0.5f, 1.5f));

		logo2->position(eae6320::Math::cVector(-1.0f, 0.5f, 1.5f));
		box2->position(eae6320::Math::cVector(-1.0f, -0.5f, 1.5f));

		logo3->position(eae6320::Math::cVector(0.0f, 0.5f, 1.5f));
		box3->position(eae6320::Math::cVector(0.0f, -0.5f, 1.5f));

		logo4->position(eae6320::Math::cVector(1.0f, 0.5f, 1.5f));
		box4->position(eae6320::Math::cVector(1.0f, -0.5f, 1.5f));

		logo5->position(eae6320::Math::cVector(15.0f, 1.5f, 0.5f));
		box5->position(eae6320::Math::cVector(15.0f, -1.5f, 0.5f));

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
		logo1.reset();
		logo2.reset();
		logo3.reset();
		logo4.reset();
		logo5.reset();
		box1.reset();
		box2.reset();
		box3.reset();
		box4.reset();
		box5.reset();

		movable.reset();
		floorObject.reset();

		green_transparent.reset();
		red_opaque.reset();
		green_opaque.reset();
		eae6320Mat.reset();
		alphaMat.reset();

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
		std::shared_ptr<Engine::GameObject> movableObject;
		
		//move the camera
		movableObject = graphics->camera()->gameObject();
		if (Keyboard::Pressed(Keyboard::W))						//forward
			movableObject->Move(eae6320::Math::cVector(0.0f, 0.0f, -movementAmount));
		if (Keyboard::Pressed(Keyboard::S))						//backward
			movableObject->Move(eae6320::Math::cVector(0.0f, 0.0f, movementAmount));
		if (Keyboard::Pressed(Keyboard::D))						//right
			movableObject->Move(eae6320::Math::cVector(movementAmount, 0.0f, 0.0f));
		if (Keyboard::Pressed(Keyboard::A))						//left
			movableObject->Move(eae6320::Math::cVector(-movementAmount, 0.0f, 0.0f));

		//move the object
		movableObject = movable;
		if (Keyboard::Pressed(Keyboard::I))						//up
			movableObject->Move(eae6320::Math::cVector(0.0f, movementAmount));
		if (Keyboard::Pressed(Keyboard::K))						//down
			movableObject->Move(eae6320::Math::cVector(0.0f, -movementAmount));
		if (Keyboard::Pressed(Keyboard::L))						//right
			movableObject->Move(eae6320::Math::cVector(movementAmount, 0.0f));
		if (Keyboard::Pressed(Keyboard::J))						//left
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
