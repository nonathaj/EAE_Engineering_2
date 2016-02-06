// Header Files
//=============

#include "Graphics.h"
#include "Context.h"
#include "RenderableComponent.h"
#include "DebugRenderer.h"
#include "DebugMenu.h"
#include "Sprite.h"
#include "FontRenderer.h"
#include "../Component/GameObject.h"
#include "../Core/Matrix4x4.h"
#include "../System/Console.h"
#include "../System/UserOutput.h"
#include "../Core/Rectangle2D.h"

namespace Lame
{
	bool Graphics::Setup(const HWND i_renderingWindow)
	{
		return Setup(std::shared_ptr<Context>(Lame::Context::Create(i_renderingWindow)));
	}

	bool Graphics::Setup(std::shared_ptr<Context> i_context)
	{
		if (!i_context || context_)
			return false;

		std::shared_ptr<Lame::GameObject> cameraGameObject(new Lame::GameObject());
		if (!cameraGameObject)
			return false;

#ifdef ENABLE_DEBUG_MENU
		std::shared_ptr<Debug::Menu> dm(Debug::Menu::Create(i_context));
		if (!dm)
		{
			UserOutput::Display("Failed to create Debug menu");
		}
#endif

		std::shared_ptr<CameraComponent> cam(new CameraComponent(cameraGameObject, i_context));
		if (!cam)
			return false;

		context_ = i_context;
		camera_ = cam;
		camera_gameobject_ = cameraGameObject;
#ifdef ENABLE_DEBUG_MENU
		debug_menu_ = dm;
#endif
		return true;
	}

	bool Graphics::Render()
	{
		bool success = context()->Clear(true, true, true) && context()->BeginFrame();
		if (!success)
			return false;

		Lame::Matrix4x4 worldToView = camera()->WorldToView();
		Lame::Matrix4x4 viewToScreen = camera()->ViewToScreen();

		std::vector<std::shared_ptr<RenderableComponent>> transparent;

		//iterate all the renderables, rendering the opaque ones first.
		for (auto itr = renderables_.begin(); itr != renderables_.end(); /**/)
		{
			std::shared_ptr<Lame::GameObject> go = (*itr)->gameObject();
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

		//render all the transparent objects on top of the opaque ones
		for (size_t x = 0; x < transparent.size(); x++)
		{
			success = transparent[x]->Render(worldToView, viewToScreen) && success;
		}

		for (auto itr = sprites_.begin(); itr != sprites_.end(); ++itr)
		{
			success = (*itr)->Render() && success;
		}

#ifdef ENABLE_DEBUG_MENU
		if (debug_menu_)
			success = debug_menu_->RenderAndUpdate() && success;
#endif

#ifdef ENABLE_DEBUG_RENDERING
		if(debug_renderer_)
			success = debug_renderer_->Render(worldToView, viewToScreen) && success;
#endif

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
