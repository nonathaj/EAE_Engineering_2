// Header Files
//=============

#include "Graphics.h"
#include "Context.h"
#include "RenderableComponent.h"
#include "../Component/GameObject.h"
#include "../Core/eae6320/cMatrix_transformation.h"

namespace Lame
{
	Graphics* Graphics::Create(std::shared_ptr<Context> i_context)
	{
		if (!i_context)
			return nullptr;

		std::shared_ptr<Engine::GameObject> cameraGameObject(new Engine::GameObject());
		if (!cameraGameObject)
			return nullptr;

		std::shared_ptr<CameraComponent> cam(new CameraComponent(cameraGameObject, i_context));
		if (!cam)
			return nullptr;
		else
			return new Graphics(i_context, cam, cameraGameObject);
	}

	Graphics::Graphics(std::shared_ptr<Context> i_context, std::shared_ptr<CameraComponent> i_camera, std::shared_ptr<Engine::GameObject> i_camera_gamebject) :
		context_(i_context),
		camera_(i_camera),
		camera_gamebject_(i_camera_gamebject),
		renderables_()
	{ }

	bool Graphics::Render()
	{
		bool success = context()->Clear(true, true, false) && context()->BeginFrame();
		if (!success)
			return false;
		eae6320::Math::cMatrix_transformation worldToView = camera()->WorldToView();
		eae6320::Math::cMatrix_transformation viewToScreen = camera()->ViewToScreen();
		for (size_t x = 0; x < renderables_.size(); x++)
		{
			success = renderables_[x]->Render(worldToView, viewToScreen) && success;
		}
		success = context()->EndFrame() && success;
		return success;
	}

	bool Graphics::MatchesContext(std::shared_ptr<RenderableComponent> i_renderable)
	{
		//check for a valid renderable
		if (!i_renderable)
			return false;

		//check the mesh or effect for a context
		std::shared_ptr<Context> ct;
		if (i_renderable->effect())
			ct = i_renderable->effect()->get_context();
		else if (i_renderable->mesh())
			ct = i_renderable->mesh()->get_context();
		else
			return false;

		//compare the context to this graphic's context
		return context() == ct;
	}

	bool Graphics::Add(std::shared_ptr<RenderableComponent> i_renderable)
	{
		if (!MatchesContext(i_renderable))
			return false;

		auto itr = std::find(renderables_.begin(), renderables_.end(), i_renderable);
		if (itr == renderables_.end())
		{
			renderables_.push_back(i_renderable);
			return true;
		}
		return false;
	}

	bool Graphics::Remove(std::shared_ptr<RenderableComponent> i_renderable)
	{
		//first check if this renderable even has the same context as us, if not, then we can't even remove it
		if (!MatchesContext(i_renderable))
			return false;

		auto itr = std::find(renderables_.begin(), renderables_.end(), i_renderable);
		if (itr != renderables_.end())
		{
			renderables_.erase(itr);
			return true;
		}
		return false;
	}
}
