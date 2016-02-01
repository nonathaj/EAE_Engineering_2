#ifndef _LAME_SPRITE_H
#define _LAME_SPRITE_H

#include <memory>

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Engine
{
	class Rectangle2D;
}

namespace Lame
{
	class Context;
	class Effect;
	class Texture;

	class Sprite
	{
	public:
		static Sprite* Create(std::shared_ptr<Context> i_context, const Engine::Rectangle2D& i_screen_pos);

		bool Render();

		inline std::shared_ptr<Context> context() { return context_; }
		inline std::shared_ptr<Effect> effect() { return effect_; }
		inline std::shared_ptr<Texture> texture() { return texture_; }
	private:
		Sprite();

		std::shared_ptr<Context> context_;
		std::shared_ptr<Effect> effect_;
		std::shared_ptr<Texture> texture_;

#if EAE6320_PLATFORM_D3D
		IDirect3DVertexBuffer9 *vertex_buffer_;
		IDirect3DVertexDeclaration9 *vertex_declaration_;
#elif EAE6320_PLATFORM_GL
		GLuint vertex_array_id_;
#endif
	};
}

#endif //_LAME_SPRITE_H