#ifndef _LAME_DEBUGRENDERER_H
#define _LAME_DEBUGRENDERER_H

#ifdef _DEBUG
#define ENABLE_DEBUG_RENDERING
#endif

#ifdef ENABLE_DEBUG_RENDERING

#include <vector>
#include <cstdint>
#include <memory>

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

#include "Vertex.h"

namespace Engine
{
	class Vector3;
	class Matrix4x4;
}

namespace Lame
{
	struct Color32;
	class Effect;

	class DebugRenderer
	{
	public:
		static DebugRenderer* Create(std::shared_ptr<Lame::Effect> i_effect, const size_t i_line_count);

		bool AddLine(const Engine::Vector3& i_start, const Engine::Vector3& i_end, const Lame::Color32& i_start_color, const Lame::Color32& i_end_color);
		bool AddLine(const Engine::Vector3& i_start, const Engine::Vector3& i_end, const Lame::Color32& i_color);

		bool Render(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen);

		std::shared_ptr<Lame::Effect> get_effect() const { return effect; }

	private:
		DebugRenderer() {}
		DebugRenderer(const DebugRenderer &i_other);
		DebugRenderer& operator=(const DebugRenderer &i_other);

		std::shared_ptr<Lame::Effect> effect;

		std::vector<Lame::Vertex> line_vertices;
		size_t max_lines_count;

#if EAE6320_PLATFORM_D3D
		IDirect3DVertexBuffer9 *vertex_buffer_;
#elif EAE6320_PLATFORM_GL
		GLuint vertex_array_id_;
#endif
	};
}

#endif //ENABLE_DEBUG_RENDERING

#endif //_LAME_DEBUGRENDERER_H
