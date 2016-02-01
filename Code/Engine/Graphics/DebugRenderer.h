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
#include "../Component/Transform.h"

namespace Engine
{
	class Vector3;
	class Quaternion;
	class Matrix4x4;
}

namespace Lame
{
	struct Color32;
	class Context;
	class Effect;
	class Mesh;

	class DebugRenderer
	{
		struct DebugMesh
		{
			Engine::Transform transform;
			std::shared_ptr<Lame::Mesh> mesh;
		};
	public:
		static DebugRenderer* Create(std::shared_ptr<Lame::Context> i_context, const size_t i_line_count);

		~DebugRenderer();

		bool AddLine(const Engine::Vector3& i_start, const Engine::Vector3& i_end, const Lame::Color32& i_start_color, const Lame::Color32& i_end_color);
		bool AddLine(const Engine::Vector3& i_start, const Engine::Vector3& i_end, const Lame::Color32& i_color);

		bool AddMesh(std::shared_ptr<Lame::Mesh> i_mesh, const Engine::Transform& i_transform, const bool i_render_as_wireframe);

		bool AddBox(const bool i_render_wireframe, const Engine::Vector3& i_size, const Engine::Transform& i_transform, const Color32& i_color = Color32::white);
		bool AddSphere(const bool i_render_wireframe, const float i_radius, const Engine::Transform& i_transform, const Color32& i_color = Color32::white);
		bool AddCylinder(const bool i_render_wireframe, const float i_top_radius, const float i_bottom_radius, const float i_height, const Engine::Transform& i_transform, const Color32& i_color = Color32::white);

		bool Render(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen);

		std::shared_ptr<Lame::Effect> get_line_effect() const { return line_effect; }
		std::shared_ptr<Lame::Effect> get_solid_shape_effect() const { return solid_shape_effect; }
		std::shared_ptr<Lame::Effect> get_wireframe_shape_effect() const { return wireframe_shape_effect; }

	private:
		DebugRenderer() {}
		DebugRenderer(const DebugRenderer &i_other);
		DebugRenderer& operator=(const DebugRenderer &i_other);

		bool RenderLines(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen);
		static bool RenderMeshes(std::shared_ptr<Lame::Effect> effect, const std::vector<DebugMesh>& i_meshes, const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen);

		std::shared_ptr<Lame::Effect> line_effect;
		std::shared_ptr<Lame::Effect> solid_shape_effect;
		std::shared_ptr<Lame::Effect> wireframe_shape_effect;

		std::vector<Lame::Vertex> line_vertices;
		size_t max_lines_count;

		std::vector<DebugMesh> wireframe_meshes;
		std::vector<DebugMesh> solid_meshes;

#if EAE6320_PLATFORM_D3D
		IDirect3DVertexBuffer9 *vertex_buffer_;
#elif EAE6320_PLATFORM_GL
		GLuint vertex_array_id_;
		GLuint vertex_buffer_id_;
#endif
	};
}

#endif //ENABLE_DEBUG_RENDERING

#endif //_LAME_DEBUGRENDERER_H
