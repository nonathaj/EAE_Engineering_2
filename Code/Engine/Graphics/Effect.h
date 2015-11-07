#ifndef _LAME_EFFECT_H
#define _LAME_EFFECT_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>

#include "../Core/eae6320/cVector.h"
#include "../Core/eae6320/cMatrix_transformation.h"
#include "../Core/HashedString.h"

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
struct ID3DXConstantTable;		//forward declare the directX constant table
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
		static Effect* Create(std::shared_ptr<Context> i_context, const std::string& i_effect_path);
		static Effect* Create(std::shared_ptr<Context> i_context, const char* i_vertex_path, const char* i_fragment_path);
		~Effect();

		bool Bind();

		bool SetLocalToWorld(eae6320::Math::cMatrix_transformation i_matrix);
		bool SetWorldToView(eae6320::Math::cMatrix_transformation i_matrix);
		bool SetViewToScreen(eae6320::Math::cMatrix_transformation i_matrix);

		//Cache a constant for dynamic setting
		bool CacheConstant(const std::string &i_constant, Engine::HashedString* o_constantId = nullptr);

		//sets the value of a cache'd constant
		bool SetConstant(const Engine::HashedString &i_constant, const eae6320::Math::cVector &i_val);
		bool SetConstant(const Engine::HashedString &i_constant, const eae6320::Math::cMatrix_transformation &i_val);

		std::shared_ptr<Context> get_context() { return context; }

	private:
		Effect(std::shared_ptr<Context> i_context) : context(i_context), constants() {}

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

		//TODO find a way to use D3DXHANDLE here instead of const char* without including more directx headers
		// we can't forwad declare the D3DXHANDLE (because it's a typedef, not a normal type), without risking screwing it up.
		typedef const char* ConstantHandle;
#elif EAE6320_PLATFORM_GL
		// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".
		GLuint programId;
		GLint positionHandle;

		typedef GLint ConstantHandle;
#else
#error No typedef for ConstantHandle
#endif
		std::unordered_map<Engine::HashedString, ConstantHandle> constants;

		static char const * const LocalToWorldUniformName;
		static const Engine::HashedString LocalToWorldUniformId;

		static char const * const WorldToViewUniformName;
		static const Engine::HashedString WorldToViewUniformId;

		static char const * const ViewToScreenUniformName;
		static const Engine::HashedString ViewToScreenUniformId;
	};
}

#endif //_LAME_EFFECT_H
