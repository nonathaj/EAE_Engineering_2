/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============

#include "../../Engine/Windows/Includes.h"
#include <memory>
#include <vector>

// Interface
//==========
namespace Lame
{
	class Context;
	class RenderableComponent;

	class Graphics
	{
	public:
		Graphics() :context_(nullptr), renderables_() {}
		Graphics(std::shared_ptr<Context> i_context);

		bool Render();

		bool Add(std::shared_ptr<RenderableComponent> i_renderable);
		bool Remove(std::shared_ptr<RenderableComponent> i_renderable);

		//does this renderable have the same context as this graphics object
		bool MatchesContext(std::shared_ptr<RenderableComponent> i_renderable);

		inline std::shared_ptr<Context> context() const { return context_; }
		inline void context(std::shared_ptr<Context> i_context) { context_ = i_context; }

	private:
		std::shared_ptr<Context> context_;
		std::vector<std::shared_ptr<RenderableComponent>> renderables_;
	};
}

#endif	// EAE6320_GRAPHICS_H
