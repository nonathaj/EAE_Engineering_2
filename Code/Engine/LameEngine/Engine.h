#ifndef _LAMEENGINE_LAME_ENGINE
#define _LAMEENGINE_LAME_ENGINE

#include <memory>

#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Graphics/RenderableComponent.h"
#include "../Graphics/Graphics.h"
#include "../Component/World.h"
#include "../../Engine/System/eae6320/Time.h"
#include "../System/UserInput.h"
#include "../System/UserOutput.h"

namespace Lame
{
	namespace Engine
	{
		bool Setup(HWND i_window);
		bool BeginFrame(float& o_deltaTime);
		bool EndFrame();
		bool Shutdown();

		Graphics* graphics();
		World* world();

		std::shared_ptr<Material> CreateMaterial(const std::string& i_material);
		std::shared_ptr<Mesh> CreateMesh(const std::string& i_mesh);
		std::shared_ptr<RenderableComponent> CreateRenderableObject(std::shared_ptr<GameObject> i_go, std::shared_ptr<Mesh> i_mesh, std::shared_ptr<Material> i_material);
	}
}

#endif //_LAMEENGINE_LAME_ENGINE
