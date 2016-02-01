#ifndef _LAME_EFFECT_H
#define _LAME_EFFECT_H

#include <cstdint>
#include <string>
#include <tuple>
#include <memory>

#include "../Core/Vector3.h"
#include "../Core/Matrix4x4.h"
#include "../Core/HashedString.h"
#include "../Core/EnumMask.h"

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
struct ID3DXConstantTable;		//forward declare the directX constant table
#elif EAE6320_PLATFORM_GL
#define LAME_EFFECT_TEXTURE_INDEX_REQUIRED_TO_SET_TEXTURE
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Lame
{
	class Context;
	class Texture;

	enum RenderState
	{
		Transparency,
		DepthTest,
		DepthWrite,
		FaceCull,
		Wireframe,
		Count
	};

	class Effect
	{
	public:
		enum Shader { Vertex, Fragment, };

#if EAE6320_PLATFORM_D3D
		//TODO find a way to use D3DXHANDLE here instead of const char* without including more directx headers
		// we can't forwad declare the D3DXHANDLE (because it's a typedef, not a normal type), without risking screwing it up.
		typedef std::tuple<const char*, DWORD> ConstantHandle;
#elif EAE6320_PLATFORM_GL
		typedef GLint ConstantHandle;
#else
#error No typedef for ConstantHandle
#endif

		static Effect* Create(std::shared_ptr<Context> i_context, const std::string& i_effect_path);
		static Effect* Create(std::shared_ptr<Context> i_context, const char* i_vertex_path, const char* i_fragment_path, Engine::EnumMask<RenderState> i_renderMask);
		~Effect();

		bool Bind();

		//Cache a constant for dynamic setting
		bool CacheConstant(const Shader &i_shader, const std::string &i_constant, ConstantHandle &o_constantId);

		//sets the value of a cache'd constant
		bool SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const Engine::Vector3 &i_val);
		bool SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const Engine::Matrix4x4 &i_val);
		bool SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const float *i_val, const size_t &i_val_count);
		bool SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const Lame::Texture *i_val
#if defined( LAME_EFFECT_TEXTURE_INDEX_REQUIRED_TO_SET_TEXTURE )
			, size_t i_index
#endif
			);

		std::shared_ptr<Context> get_context() { return context; }
		Engine::EnumMask<RenderState> render_mask() { return renderMask; }

		bool has_transparency() { return renderMask.test(RenderState::Transparency); }
		void has_transparency(const bool i_val) { renderMask.set(RenderState::Transparency, i_val); }

		bool has_depth_test() { return renderMask.test(RenderState::DepthTest); }
		void has_depth_test(const bool i_val) { renderMask.set(RenderState::DepthTest, i_val); }
		
		bool has_depth_write() { return renderMask.test(RenderState::DepthWrite); }
		void has_depth_write(const bool i_val) { renderMask.set(RenderState::DepthWrite, i_val); }
		
		bool has_face_cull() { return renderMask.test(RenderState::FaceCull); }
		void has_face_cull(const bool i_val) { renderMask.set(RenderState::FaceCull, i_val); }

		bool is_wireframe() { return renderMask.test(RenderState::Wireframe); }
		void is_wireframe(const bool i_val) { renderMask.set(RenderState::Wireframe, i_val); }

	private:
		Effect(std::shared_ptr<Context> i_context, Engine::EnumMask<RenderState> i_renderMask) : context(i_context), renderMask(i_renderMask) {}

		//Do not allow Effects to be managed without pointers
		Effect();
		Effect(const Effect &i_mesh);
		Effect& operator=(const Effect &i_mesh);

		std::shared_ptr<Context> context;
		Engine::EnumMask<RenderState> renderMask;
#if EAE6320_PLATFORM_D3D
		IDirect3DVertexShader9 *vertexShader;
		IDirect3DPixelShader9 *fragmentShader;

		ID3DXConstantTable *vertexConstantTable;
		ID3DXConstantTable *fragmentConstantTable;

		ID3DXConstantTable* get_constant_table(const Shader &i_shader) { return i_shader == Shader::Vertex ? vertexConstantTable : fragmentConstantTable; }
#elif EAE6320_PLATFORM_GL
		// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".
		GLuint programId;
		GLint positionHandle;
#endif
	};
}

#endif //_LAME_EFFECT_H
