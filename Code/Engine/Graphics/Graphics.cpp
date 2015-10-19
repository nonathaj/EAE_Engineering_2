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
		bool success = context()->BeginFrame();
		if (!success)
			return false;
		for (size_t x = 0; x < renderables_.size(); x++)
		{
			success = renderables_[x]->Render() && success;
		}
		success = context()->EndFrame() && success;
		return success;
	}

	bool Graphics::Add(std::shared_ptr<RenderableComponent> i_renderable)
	{
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
		auto itr = std::find(renderables_.begin(), renderables_.end(), i_renderable);
		if (itr != renderables_.end())
		{
			renderables_.erase(itr);
			return true;
		}
		return false;
	}
}