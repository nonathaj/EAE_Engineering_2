#ifndef LAME_GRAPHICS_H
#define LAME_GRAPHICS_H

#include <memory>
#include <vector>
#include <string>

#include "../../Engine/Windows/Includes.h"
#include "CameraComponent.h"
#include "DebugRenderer.h"

namespace Lame
{
	class Context;
	class RenderableComponent;
	class Effect;

	class Graphics
	{
	public:
		static Graphics* Create(const HWND i_renderingWindow);
		static Graphics* Create(std::shared_ptr<Context> i_context);

		bool Render();

		bool Add(std::shared_ptr<RenderableComponent> i_renderable);
		bool Remove(std::shared_ptr<RenderableComponent> i_renderable);

		//does this renderable have the same context as this graphics object
		bool MatchesContext(std::shared_ptr<RenderableComponent> i_renderable);

		inline std::shared_ptr<Context> context() const { return context_; }
		inline std::shared_ptr<CameraComponent> camera() const { return camera_; }
		
#ifdef ENABLE_DEBUG_RENDERING
		bool EnableDebugDrawing(const std::string& i_effect_path, const size_t i_line_count);

		DebugRenderer* debug_renderer() const { return debug_renderer_.get(); }
#endif

	private:
		Graphics(std::shared_ptr<Context> i_context, std::shared_ptr<CameraComponent> i_camera, std::shared_ptr<Engine::GameObject> i_camera_gamebject);

		//Do not allow Graphics to be managed without pointers
		Graphics();
		Graphics(const Graphics &i_other);
		Graphics& operator=(const Graphics &i_other);

		std::shared_ptr<Context> context_;
		std::vector<std::shared_ptr<RenderableComponent>> renderables_;

#ifdef ENABLE_DEBUG_RENDERING
		std::shared_ptr<DebugRenderer> debug_renderer_;
#endif

		//store the camera's gameobject, so it lives for the duration of this graphics
		std::shared_ptr<Engine::GameObject> camera_gamebject_;
		std::shared_ptr<CameraComponent> camera_;
	};

	namespace Shader
	{
		enum Type { Unknown, Vertex, Fragment, };
	}
}

#endif	// LAME_GRAPHICS_H
