#ifndef _LAME_EFFECT_H
#define _LAME_EFFECT_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>

#include "../Core/Vector2.h"
#include "../Core/HashedString.h"

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
		static Effect* Create(std::shared_ptr<Context> i_context, std::string i_vertex_path, std::string i_fragment_path);
		~Effect();

		bool Bind();
		bool SetPosition(Engine::Vector2 i_position);

		//Cache a constant for dynamic setting
		bool CacheConstant(const std::string &i_constant, Engine::HashedString* o_constantId = nullptr);

		//sets the value of a cache'd constant
		bool SetConstant(const Engine::HashedString &i_constant, const Engine::Vector2 &i_val);

	private:
		Effect(std::shared_ptr<Context> i_context) : context(i_context) {}

		//Do not allow Effects to be managed without pointers
		Effect();
		Effect(const Effect &i_mesh);
		Effect& operator=(const Effect &i_mesh);

		std::shared_ptr<Context> context;
#if EAE6320_PLATFORM_D3D
		IDirect3DVertexShader9 *vertexShader;
		IDirect3DPixelShader9 *fragmentShader;
		ID3DXConstantTable *vertexConstantTable;
		ID3DXConstantTable *fragmentConstantTable;

		typedef D3DXHANDLE ConstantHandle;
#elif EAE6320_PLATFORM_GL
		// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".
		GLuint programId;
		GLint positionHandle;

		typedef GLint ConstantHandle;
#else
#error No typedef for ConstantHandle
#endif
		std::unordered_map<Engine::HashedString, ConstantHandle> constants;

		static char const * const PositionUniformName;
		static const Engine::HashedString PositionUniformId;
	};
}

#endif //_LAME_EFFECT_H
