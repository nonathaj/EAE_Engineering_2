
#include "Gameplay.h"

#include <vector>
#include <string>

#include "../../Engine/Graphics/Context.h"
#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/Effect.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Core/Singleton.h"
#include "../../Engine/System/Time.h"
#include "../../Engine/Component/GameObject.h"
#include "../../Engine/Graphics/RenderableComponent.h"

namespace
{
	//the graphics singleton
	typedef Engine::Singleton<Lame::Graphics> Graphics;

	std::shared_ptr<Engine::GameObject> CreateObject(std::string i_mesh);

	//gameobject
	std::shared_ptr<Engine::GameObject> square;
	std::shared_ptr<Engine::GameObject> triangle1;
	std::shared_ptr<Engine::GameObject> triangle2;
}

namespace Gameplay
{
	bool Initialize(HWND i_window)
	{
		//generate a new graphics object
		Graphics::Release();
		Graphics::Get().context(std::shared_ptr<Lame::Context>(Lame::Context::Create(i_window)));
		if (!Graphics::Get().context()) //if we failed to generate a graphics context, shutdown the game
		{
			Shutdown();
			return false;
		}

		//Create our renderables
		square = CreateObject("data/square.mesh");
		triangle1 = CreateObject("data/triangle.mesh");
		triangle2 = CreateObject("data/triangle.mesh");
		if (!square || !triangle1 || !triangle2)
		{
			Shutdown();
			return false;
		}
		
		Engine::Time::Setup();

		return true;
	}

	bool RunFrame()
	{
		Engine::Time::Service();
		double deltaTime = Engine::Time::DeltaTime();

		//TODO
		//game logic here

		bool renderSuccess = Graphics::Get().Render();
		return renderSuccess;
	}

	bool Shutdown()
	{
		square.reset();
		triangle1.reset();
		triangle2.reset();

		Graphics::Release();
		return true;
	}
}

namespace
{
	std::shared_ptr<Engine::GameObject> CreateObject(std::string i_mesh)
	{
		std::shared_ptr<Lame::Mesh> mesh(Lame::Mesh::Create(Graphics::Get().context(), i_mesh));
		if (!mesh)
			return nullptr;

		std::shared_ptr<Lame::Effect> effect(Lame::Effect::Create(Graphics::Get().context(), "data/vertex.shader", "data/fragment.shader"));
		if (!effect)
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
		if (!Graphics::Get().Add(renderable))
			return nullptr;

		return go;
	}
}