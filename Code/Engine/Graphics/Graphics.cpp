// Header Files
//=============

#include "Graphics.h"
#include "Context.h"
#include "RenderableComponent.h"

namespace Lame
{
	Graphics::Graphics(std::shared_ptr<Context> i_context) :
		context_(i_context), renderables_()
	{
	}

	bool Graphics::Render()
	{
		bool success = context()->Clear(true, true, false) && context()->BeginFrame();
		if (!success)
			return false;
		for (size_t x = 0; x < renderables_.size(); x++)
		{
			success = renderables_[x]->Render() && success;
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
