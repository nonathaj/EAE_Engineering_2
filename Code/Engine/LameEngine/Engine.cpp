
#include "Engine.h"

namespace
{
	std::unique_ptr<Lame::Graphics> _graphics;
	std::unique_ptr<Lame::World> _world;
}

namespace Lame
{
	namespace Engine
	{
		bool Setup(HWND i_window)
		{
			_world = std::unique_ptr<World>(new World());
			if (!_world)
			{
				Shutdown();
				return false;
			}

			_graphics = std::unique_ptr<Graphics>(Graphics::Create(i_window));
			if (!_graphics)
			{
				Shutdown();
				return false;
			}

#ifdef ENABLE_DEBUG_RENDERING
			//enable debug drawing for graphics
			if (!_graphics->EnableDebugDrawing(10000))
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

			return true;
		}

		bool BeginFrame(float& o_deltaTime)
		{
			eae6320::Time::OnNewFrame();
			o_deltaTime = eae6320::Time::GetSecondsElapsedThisFrame();
			_world->Update(o_deltaTime);
			return true;
		}

		bool EndFrame()
		{
			return _graphics->Render();
		}

		bool Shutdown()
		{
			_graphics.reset();
			_world.reset();
			return true;
		}

		Lame::Graphics* graphics()
		{
			return _graphics.get();
		}

		Lame::World* world()
		{
			return _world.get();
		}

		std::shared_ptr<Material> CreateMaterial(const std::string& i_material)
		{
			if (!_graphics || !_graphics->context())
				return nullptr;
			return std::shared_ptr<Material>(Material::Create(_graphics->context(), i_material));
		}

		std::shared_ptr<Mesh> CreateMesh(const std::string& i_mesh)
		{
			if (!_graphics || !_graphics->context())
				return nullptr;
			return std::shared_ptr<Mesh>(Mesh::Create(_graphics->context(), i_mesh));
		}

		std::shared_ptr<RenderableComponent> CreateRenderableObject(std::shared_ptr<GameObject> i_go, std::shared_ptr<Mesh> i_mesh, std::shared_ptr<Material> i_material)
		{
			if (!_world || !_graphics || !_graphics->context() || !i_mesh || !i_material)
				return nullptr;

			//create the gameObject, if we did not receive a valid one
			std::shared_ptr<GameObject> go = i_go;
			if (!go)
			{
				go = _world->AddNewGameObject();
				if (!go)
					return nullptr;
			}
			else
			{
				if (!_world->Add(go))
					return nullptr;
			}

			//create the renderable
			std::shared_ptr<RenderableComponent> renderable(RenderableComponent::Create(go, i_mesh, i_material));
			if (!renderable)
			{
				_world->Remove(go);
				return nullptr;
			}

			//add the renderable to the graphics system
			if (!_graphics->Add(renderable))
			{
				_world->Remove(go);
				return nullptr;
			}

			return renderable;
		}
	}
}
