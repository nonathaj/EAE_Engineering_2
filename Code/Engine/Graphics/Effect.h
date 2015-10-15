#ifndef _LAME_EFFECT_H
#define _LAME_EFFECT_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "../Core/Vector2.h"

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#include <D3DX9Shader.h>
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
		static Effect* Create(Context *& i_context, std::string i_vertex_path, std::string i_fragment_path);
		~Effect();

		bool Bind();

		bool SetPosition(Engine::Vector2 i_position);

	private:
		//Do not allow Effects to be managed without pointers
		Effect();
		Effect(Context *& i_context) : context(i_context) {}
		Effect(const Effect &i_mesh);
		Effect& operator=(const Effect &i_mesh);

		Context *& context;
#if EAE6320_PLATFORM_D3D
		IDirect3DVertexShader9 *vertexShader;
		IDirect3DPixelShader9 *fragmentShader;
		ID3DXConstantTable *vertexConstantTable;
		ID3DXConstantTable *fragmentConstantTable;
		D3DXHANDLE positionHandle;
#elif EAE6320_PLATFORM_GL
		// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".
		GLuint programId;
		GLint positionHandle;
#endif
		static char const * const PositionUniformName;
	};
}

#endif //_LAME_EFFECT_H
