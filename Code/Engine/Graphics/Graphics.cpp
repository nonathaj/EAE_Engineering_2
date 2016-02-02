// Header Files
//=============

#include "Graphics.h"
#include "Context.h"
#include "RenderableComponent.h"
#include "DebugRenderer.h"
#include "Sprite.h"
#include "../Component/GameObject.h"
#include "../Core/Matrix4x4.h"
#include "../System/Console.h"
#include "../System/UserOutput.h"

namespace Lame
{
	Graphics* Graphics::Create(const HWND i_renderingWindow)
	{
		return Create(std::shared_ptr<Context>(Lame::Context::Create(i_renderingWindow)));
	}

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
#ifdef ENABLE_DEBUG_RENDERING
		, debug_renderer_(nullptr)
#endif
	{ }

	bool Graphics::Render()
	{
		bool success = context()->Clear(true, true, false) && context()->BeginFrame();
		if (!success)
			return false;

		Engine::Matrix4x4 worldToView = camera()->WorldToView();
		Engine::Matrix4x4 viewToScreen = camera()->ViewToScreen();

		std::vector<std::shared_ptr<RenderableComponent>> transparent;

		//iterate all the renderables, rendering the opaque ones first.
		for (auto itr = renderables_.begin(); itr != renderables_.end(); /**/)
		{
			std::shared_ptr<Engine::GameObject> go = (*itr)->gameObject();
			if (go && !go->IsDestroying())
			{
				if ((*itr)->material()->effect()->has_transparency())
					transparent.push_back(*itr);
				else
				{
					success = (*itr)->Render(worldToView, viewToScreen) && success;
				}
				++itr;
			}
			else
			{
				itr = renderables_.erase(itr);
			}
		}

#ifdef ENABLE_DEBUG_RENDERING
		success = debug_renderer_->Render(worldToView, viewToScreen) && success;
#endif

		//render all the transparent objects on top of the opaque ones
		for (size_t x = 0; x < transparent.size(); x++)
		{
			success = transparent[x]->Render(worldToView, viewToScreen) && success;
		}

		for (auto itr = sprites_.begin(); itr != sprites_.end(); ++itr)
		{
			success = (*itr)->Render();
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
		if (i_renderable->mesh())
			ct = i_renderable->mesh()->get_context();
		else if (i_renderable->material() && i_renderable->material()->effect())
			ct = i_renderable->material()->effect()->get_context();
		else
			return false;

		//compare the context to this graphic's context
		return context() == ct;
	}

	bool Graphics::Add(std::shared_ptr<RenderableComponent> i_renderable)
	{
		if (!i_renderable || !MatchesContext(i_renderable))
			return false;

		auto itr = std::find(renderables_.begin(), renderables_.end(), i_renderable);
		if (itr == renderables_.end())
		{
			renderables_.push_back(i_renderable);
			return true;
		}
		return false;
	}

	bool Graphics::Add(std::shared_ptr<Lame::Sprite> i_sprite)
	{
		if (!i_sprite || i_sprite->effect()->get_context() != context())
			return false;

		auto itr = std::find(sprites_.begin(), sprites_.end(), i_sprite);
		if (itr == sprites_.end())
		{
			sprites_.push_back(i_sprite);
			return true;
		}
		return false;
	}

	bool Graphics::Remove(std::shared_ptr<RenderableComponent> i_renderable)
	{
		//first check if this renderable even has the same context as us, if not, then we can't even remove it
		if (i_renderable || !MatchesContext(i_renderable))
			return false;

		auto itr = std::find(renderables_.begin(), renderables_.end(), i_renderable);
		if (itr != renderables_.end())
		{
			renderables_.erase(itr);
			return true;
		}
		return false;
	}

	bool Graphics::Remove(std::shared_ptr<Sprite> i_sprite)
	{
		if (!i_sprite || i_sprite->effect()->get_context() != context())
			return false;

		auto itr = std::find(sprites_.begin(), sprites_.end(), i_sprite);
		if (itr != sprites_.end())
		{
			sprites_.erase(itr);
			return true;
		}
		return false;
	}

#ifdef ENABLE_DEBUG_RENDERING
	bool Graphics::EnableDebugDrawing(const size_t i_line_count)
	{
		if (!debug_renderer_)
		{
			debug_renderer_ = std::shared_ptr<DebugRenderer>(DebugRenderer::Create(context_, i_line_count));
			return debug_renderer_ != nullptr;
		}
		else
		{
			return false;
		}
	}
#endif //ENABLE_DEBUG_RENDERING
}
