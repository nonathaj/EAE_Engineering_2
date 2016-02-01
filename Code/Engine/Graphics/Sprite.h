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

	class Sprite
	{
	public:

		inline std::shared_ptr<Context> get_context() { return context; }
	private:
		std::shared_ptr<Context> context;

#if EAE6320_PLATFORM_D3D
		IDirect3DVertexBuffer9 *vertex_buffer_;
		IDirect3DVertexDeclaration9 *vertex_declaration_;
#elif EAE6320_PLATFORM_GL
		GLuint vertex_array_id_;
#endif
	};
}

#endif //_LAME_SPRITE_H