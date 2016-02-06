#ifndef _LAME_DEBUGRENDERER_H
#define _LAME_DEBUGRENDERER_H

#ifdef _DEBUG
#define ENABLE_DEBUG_RENDERING
#endif

#ifdef ENABLE_DEBUG_RENDERING

#include <vector>
#include <cstdint>
#include <memory>

#include "Vertex.h"
#include "Effect.h"
#include "Context.h"
#include "../Component/Transform.h"

namespace Lame
{
	class Vector3;
	class Quaternion;
	class Matrix4x4;
	class Color32;
	class Mesh;

	class DebugRenderer
	{
		struct DebugMesh
		{
			Lame::Transform transform;
			std::shared_ptr<Lame::Mesh> mesh;
		};
	public:
		static DebugRenderer* Create(std::shared_ptr<Lame::Context> i_context, const size_t i_line_count);

		bool AddLine(const Lame::Vector3& i_start, const Lame::Vector3& i_end, const Lame::Color32& i_start_color, const Lame::Color32& i_end_color);
		bool AddLine(const Lame::Vector3& i_start, const Lame::Vector3& i_end, const Lame::Color32& i_color);

		bool AddMesh(std::shared_ptr<Lame::Mesh> i_mesh, const Lame::Transform& i_transform, const bool i_render_as_wireframe);

		bool AddBox(const bool i_render_wireframe, const Lame::Vector3& i_size, const Lame::Transform& i_transform, const Color32& i_color = Color32::white);
		bool AddSphere(const bool i_render_wireframe, const float i_radius, const Lame::Transform& i_transform, const Color32& i_color = Color32::white);
		bool AddCylinder(const bool i_render_wireframe, const float i_top_radius, const float i_bottom_radius, const float i_height, const Lame::Transform& i_transform, const Color32& i_color = Color32::white);

		bool Render(const Lame::Matrix4x4& i_worldToView, const Lame::Matrix4x4& i_viewToScreen);

		std::shared_ptr<Lame::Effect> get_line_effect() const { return line_effect; }
		std::shared_ptr<Lame::Effect> get_solid_shape_effect() const { return solid_shape_effect; }
		std::shared_ptr<Lame::Effect> get_wireframe_shape_effect() const { return wireframe_shape_effect; }
		std::shared_ptr<Context> get_context() const { return context; }

	private:
		DebugRenderer() {}
		DebugRenderer(const DebugRenderer &i_other);
		DebugRenderer& operator=(const DebugRenderer &i_other);

		bool RenderLines(const Lame::Matrix4x4& i_worldToView, const Lame::Matrix4x4& i_viewToScreen);
		bool RenderSolidMeshes(const Lame::Matrix4x4& i_worldToView, const Lame::Matrix4x4& i_viewToScreen);
		bool RenderWireframeMeshes(const Lame::Matrix4x4& i_worldToView, const Lame::Matrix4x4& i_viewToScreen);

		std::shared_ptr<Effect> line_effect;
		std::shared_ptr<Effect> solid_shape_effect;
		std::shared_ptr<Effect> wireframe_shape_effect;

		std::shared_ptr<Context> context;

		std::vector<Lame::Vertex> line_vertices;
		size_t max_lines_count;

		std::shared_ptr<Lame::Mesh> line_renderer;

		std::vector<DebugMesh> wireframe_meshes;
		std::vector<DebugMesh> solid_meshes;

		Effect::ConstantHandle line_worldToViewUniformId;
		Effect::ConstantHandle line_viewToScreenUniformId;

		Effect::ConstantHandle wire_localToWorldUniformId;
		Effect::ConstantHandle wire_worldToViewUniformId;
		Effect::ConstantHandle wire_viewToScreenUniformId;

		Effect::ConstantHandle solid_localToWorldUniformId;
		Effect::ConstantHandle solid_worldToViewUniformId;
		Effect::ConstantHandle solid_viewToScreenUniformId;

		static char const * const LocalToWorldUniformName;
		static char const * const WorldToViewUniformName;
		static char const * const ViewToScreenUniformName;
	};
}

#endif //ENABLE_DEBUG_RENDERING

#endif //_LAME_DEBUGRENDERER_H
