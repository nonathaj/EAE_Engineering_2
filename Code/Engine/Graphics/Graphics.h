#ifndef LAME_GRAPHICS_H
#define LAME_GRAPHICS_H

#include <memory>
#include <vector>
#include <string>

#include "../../Engine/Windows/Includes.h"

#include "../Core/Singleton.h"

#include "CameraComponent.h"
#include "DebugRenderer.h"
#include "DebugMenu.h"

namespace Lame
{
	class Context;
	class RenderableComponent;
	class Effect;
	class Sprite;
	class Rectangle2D;

	namespace Shader
	{
		enum Type { Unknown, Vertex, Fragment, };
	}

	class Graphics
	{
	public:
		bool Setup(const HWND i_renderingWindow);
		bool Setup(std::shared_ptr<Context> i_context);

		bool Render();

		bool Add(std::shared_ptr<RenderableComponent> i_renderable);
		bool Remove(std::shared_ptr<RenderableComponent> i_renderable);

		bool Add(std::shared_ptr<Sprite> i_sprite);
		bool Remove(std::shared_ptr<Sprite> i_sprite);

		//does this renderable have the same context as this graphics object
		bool MatchesContext(std::shared_ptr<RenderableComponent> i_renderable);

		inline std::shared_ptr<Context> context() const { return context_; }
		inline std::shared_ptr<CameraComponent> camera() const { return camera_; }
		
#ifdef ENABLE_DEBUG_RENDERING
		bool EnableDebugDrawing(const size_t i_line_count);
		DebugRenderer* debug_renderer() const { return debug_renderer_.get(); }
#endif

#ifdef ENABLE_DEBUG_MENU
		Debug::Menu* debug_menu() const { return debug_menu_.get(); }
#endif
	private:
		Graphics() {}

		std::shared_ptr<Context> context_;
		std::vector<std::shared_ptr<RenderableComponent>> renderables_;
		std::vector<std::shared_ptr<Lame::Sprite>> sprites_;

#ifdef ENABLE_DEBUG_RENDERING
		std::shared_ptr<DebugRenderer> debug_renderer_;
#endif

#ifdef ENABLE_DEBUG_MENU
		std::shared_ptr<Debug::Menu> debug_menu_;
#endif
		//store the camera's gameobject, so it lives for the duration of this graphics
		std::shared_ptr<Lame::GameObject> camera_gameobject_;
		std::shared_ptr<CameraComponent> camera_;

		friend Lame::Singleton<Lame::Graphics>;
	};
}

typedef Lame::Singleton<Lame::Graphics> LameGraphics;

#endif	// LAME_GRAPHICS_H
