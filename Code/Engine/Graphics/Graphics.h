/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============

#include <memory>
#include <vector>

#include "../../Engine/Windows/Includes.h"
#include "CameraComponent.h"

// Interface
//==========
namespace Lame
{
	class Context;
	class RenderableComponent;

	class Graphics
	{
	public:
		static Graphics* Create(std::shared_ptr<Context> i_context);

		bool Render();

		bool Add(std::shared_ptr<RenderableComponent> i_renderable);
		bool Remove(std::shared_ptr<RenderableComponent> i_renderable);

		//does this renderable have the same context as this graphics object
		bool MatchesContext(std::shared_ptr<RenderableComponent> i_renderable);

		inline std::shared_ptr<Context> context() const { return context_; }
		inline void context(std::shared_ptr<Context> i_context) { context_ = i_context; }

		inline std::shared_ptr<CameraComponent> camera() const { return camera_; }

	private:
		Graphics(std::shared_ptr<Context> i_context, std::shared_ptr<CameraComponent> i_camera, std::shared_ptr<Engine::GameObject> i_camera_gamebject);

		//Do not allow Graphics to be managed without pointers
		Graphics();
		Graphics(const Graphics &i_other);
		Graphics& operator=(const Graphics &i_other);

		//store the camera's gameobject, so it lives for the duration of this graphics
		std::shared_ptr<Engine::GameObject> camera_gamebject_;
		std::shared_ptr<CameraComponent> camera_;
		std::shared_ptr<Context> context_;
		std::vector<std::shared_ptr<RenderableComponent>> renderables_;
	};

	namespace Shader
	{
		enum Type { Unknown, Vertex, Fragment, };
	}
}

#endif	// EAE6320_GRAPHICS_H
