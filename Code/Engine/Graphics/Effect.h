#ifndef _LAME_EFFECT_H
#define _LAME_EFFECT_H

#include <cstdint>
#include <string>

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Lame
{
	class Context;

	class Effect
	{
	public:
		~Effect();

		static Effect* Create(Context *& i_context, std::string i_vertex_path, std::string i_fragment_path);
		static void Destroy(Effect *i_effect);

		bool Bind();

	private:
		//Do not allow Effects to be managed without pointers
		Effect();
		Effect(Context *& i_context) : context(i_context) {}
		Effect(const Effect &i_mesh);
		Effect& operator=(const Effect &i_mesh);

		Context *& context;
#if EAE6320_PLATFORM_D3D
		IDirect3DVertexShader9 *vertexShader;
		IDirect3DPixelShader9 *pixelShader;
#elif EAE6320_PLATFORM_GL
		// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".
		GLuint programId;
#endif
	};
}

#endif //_LAME_EFFECT_H
