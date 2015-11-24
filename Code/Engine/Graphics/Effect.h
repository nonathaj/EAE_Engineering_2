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

	typedef uint8_t RenderMask;
	enum RenderState
	{
		Transparency	= 1 << 0,
		DepthTest		= 1 << 1,
		DepthWrite		= 1 << 2,
		FaceCull		= 1 << 3,
	};

	class Effect
	{
	public:
		enum Shader { Vertex, Fragment, };

		static Effect* Create(std::shared_ptr<Context> i_context, const std::string& i_effect_path);
		static Effect* Create(std::shared_ptr<Context> i_context, const char* i_vertex_path, const char* i_fragment_path, RenderMask i_renderMask);
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
		bool SetConstant(const Engine::HashedString &i_constant, const float &i_val);
		bool SetConstant(const Engine::HashedString &i_constant, const float (&i_val)[2]);
		bool SetConstant(const Engine::HashedString &i_constant, const float (&i_val)[3]);
		bool SetConstant(const Engine::HashedString &i_constant, const float (&i_val)[4]);

		std::shared_ptr<Context> get_context() { return context; }
		RenderMask render_mask() { return renderMask; }

		bool has_transparency() { return (renderMask & RenderState::Transparency) > 0; }
		void has_transparency(const bool& i_val) { i_val ? renderMask |= RenderState::Transparency : renderMask &= ~RenderState::Transparency; }

		bool has_depth_test() { return (renderMask & RenderState::DepthTest) > 0; }
		void has_depth_test(const bool& i_val) { i_val ? renderMask |= RenderState::DepthTest : renderMask &= ~RenderState::DepthTest; }
		
		bool has_depth_write() { return (renderMask & RenderState::DepthWrite) > 0; }
		void has_depth_write(const bool& i_val) { i_val ? renderMask |= RenderState::DepthWrite : renderMask &= ~RenderState::DepthWrite; }
		
		bool has_face_cull() { return (renderMask & RenderState::FaceCull) > 0; }
		void has_face_cull(const bool& i_val) { i_val ? renderMask |= RenderState::FaceCull : renderMask &= ~RenderState::FaceCull; }


#if EAE6320_PLATFORM_D3D
		//TODO find a way to use D3DXHANDLE here instead of const char* without including more directx headers
		// we can't forwad declare the D3DXHANDLE (because it's a typedef, not a normal type), without risking screwing it up.
		typedef const char* ConstantHandle;
#elif EAE6320_PLATFORM_GL
		typedef GLint ConstantHandle;
#else
#error No typedef for ConstantHandle
#endif

	private:
		Effect(std::shared_ptr<Context> i_context, RenderMask i_renderMask) : context(i_context), renderMask(i_renderMask), constants() {}

		//Do not allow Effects to be managed without pointers
		Effect();
		Effect(const Effect &i_mesh);
		Effect& operator=(const Effect &i_mesh);

		std::shared_ptr<Context> context;
		RenderMask renderMask;
#if EAE6320_PLATFORM_D3D
		IDirect3DVertexShader9 *vertexShader;
		IDirect3DPixelShader9 *fragmentShader;

		ID3DXConstantTable *vertexConstantTable;
		ID3DXConstantTable *fragmentConstantTable;
#elif EAE6320_PLATFORM_GL
		// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".
		GLuint programId;
		GLint positionHandle;
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
