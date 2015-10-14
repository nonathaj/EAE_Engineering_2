#ifndef _LAME_EFFECT_H
#define _LAME_EFFECT_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "../Core/HashedString.h"
#include "../Core/Vector2.h"
#include "../Core/Vector3.h"

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

		bool CacheConstant(const std::string &i_constant, Engine::HashedString* o_constantId = nullptr);
		bool SetConstant(const Engine::HashedString &i_constant, const Engine::Vector2 &i_val);

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

		typedef D3DXHANDLE ConstantHandle;
#elif EAE6320_PLATFORM_GL
		// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".
		GLuint programId;

		typedef GLint ConstantHandle;
#else
#error No typedef for ConstantHandle
#endif

		std::unordered_map<Engine::HashedString, ConstantHandle> constants;
	};
}

#endif //_LAME_EFFECT_H
