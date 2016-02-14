
#include "DebugRenderer.h"

#ifdef ENABLE_DEBUG_RENDERING

#include "../Core/Vertex.h"
#include "Effect.h"
#include "Context.h"
#include "RenderableMesh.h"
#include "../Core/Matrix4x4.h"
#include "../System/UserOutput.h"
#include "../Core/Quaternion.h"
#include "../System/Console.h"
#include "../Core/EnumMask.h"
#include "../Core/Mesh.h"

namespace Lame
{
	char const * const DebugRenderer::LocalToWorldUniformName = "local_to_world";
	char const * const DebugRenderer::WorldToViewUniformName = "world_to_view";
	char const * const DebugRenderer::ViewToScreenUniformName = "view_to_screen";

	DebugRenderer* DebugRenderer::Create(std::shared_ptr<Lame::Context> i_context, const size_t i_line_count)
	{
		if (!i_context)
			return nullptr;

		std::shared_ptr<Lame::Effect> line_effect;
		Effect::ConstantHandle line_worldToViewUniformId;
		Effect::ConstantHandle line_viewToScreenUniformId;
		{
			const char * const vertex_shader = "data/debug/line_vertex.shader.bin";
			const char * const fragment_shader = "data/debug/line_fragment.shader.bin";
			Lame::EnumMask<Lame::RenderState> rendermask;
			rendermask.set(Lame::RenderState::Transparency, false);
			rendermask.set(Lame::RenderState::DepthTest, true);
			rendermask.set(Lame::RenderState::DepthWrite, true);
			rendermask.set(Lame::RenderState::FaceCull, true);
			rendermask.set(Lame::RenderState::Wireframe, true);
			line_effect = std::shared_ptr<Lame::Effect>(Lame::Effect::Create(i_context, vertex_shader, fragment_shader, rendermask));
			if (!line_effect ||
				!line_effect->CacheConstant(Lame::Effect::Shader::Vertex, WorldToViewUniformName, line_worldToViewUniformId) ||
				!line_effect->CacheConstant(Lame::Effect::Shader::Vertex, ViewToScreenUniformName, line_viewToScreenUniformId))
			{
				Lame::UserOutput::Display("Failed to create debug line effect");
				return nullptr;
			}
		}

		std::shared_ptr<Lame::Effect> wireframe_shape_effect;
		Effect::ConstantHandle wire_localToWorldUniformId;
		Effect::ConstantHandle wire_worldToViewUniformId;
		Effect::ConstantHandle wire_viewToScreenUniformId;
		{
			const char * const vertex_shader = "data/debug/shape_vertex.shader.bin";
			const char * const fragment_shader = "data/debug/shape_fragment.shader.bin";
			Lame::EnumMask<Lame::RenderState> rendermask;
			rendermask.set(Lame::RenderState::Transparency, false);
			rendermask.set(Lame::RenderState::DepthTest, true);
			rendermask.set(Lame::RenderState::DepthWrite, true);
			rendermask.set(Lame::RenderState::FaceCull, false);
			rendermask.set(Lame::RenderState::Wireframe, true);
			wireframe_shape_effect = std::shared_ptr<Lame::Effect>(Lame::Effect::Create(i_context, vertex_shader, fragment_shader, rendermask));
			if (!wireframe_shape_effect ||
				!wireframe_shape_effect->CacheConstant(Lame::Effect::Shader::Vertex, LocalToWorldUniformName, wire_localToWorldUniformId) ||
				!wireframe_shape_effect->CacheConstant(Lame::Effect::Shader::Vertex, WorldToViewUniformName, wire_worldToViewUniformId) ||
				!wireframe_shape_effect->CacheConstant(Lame::Effect::Shader::Vertex, ViewToScreenUniformName, wire_viewToScreenUniformId))
			{
				Lame::UserOutput::Display("Failed to create debug wireframe effect");
				return nullptr;
			}
		}

		std::shared_ptr<Lame::Effect> fill_shape_effect;
		Effect::ConstantHandle solid_localToWorldUniformId;
		Effect::ConstantHandle solid_worldToViewUniformId;
		Effect::ConstantHandle solid_viewToScreenUniformId;
		{
			const char * const vertex_shader = "data/debug/shape_vertex.shader.bin";
			const char * const fragment_shader = "data/debug/shape_fragment.shader.bin";
			Lame::EnumMask<Lame::RenderState> rendermask;
			rendermask.set(Lame::RenderState::Transparency, true);
			rendermask.set(Lame::RenderState::DepthTest, true);
			rendermask.set(Lame::RenderState::DepthWrite, true);
			rendermask.set(Lame::RenderState::FaceCull, true);
			rendermask.set(Lame::RenderState::Wireframe, false);
			fill_shape_effect = std::shared_ptr<Lame::Effect>(Lame::Effect::Create(i_context, vertex_shader, fragment_shader, rendermask));
			if (!fill_shape_effect ||
				!fill_shape_effect->CacheConstant(Lame::Effect::Shader::Vertex, LocalToWorldUniformName, solid_localToWorldUniformId) ||
				!fill_shape_effect->CacheConstant(Lame::Effect::Shader::Vertex, WorldToViewUniformName, solid_worldToViewUniformId) ||
				!fill_shape_effect->CacheConstant(Lame::Effect::Shader::Vertex, ViewToScreenUniformName, solid_viewToScreenUniformId))
			{
				Lame::UserOutput::Display("Failed to create debug filled shape effect");
				return nullptr;
			}
		}

		std::shared_ptr<Lame::RenderableMesh> line_renderer(Lame::RenderableMesh::CreateEmpty(false, i_context, Lame::Mesh::PrimitiveType::LineList, i_line_count * 2, 0));
		if (!line_renderer)
		{
			Lame::UserOutput::Display("Failed to create debug line mesh");
			return nullptr;
		}

		DebugRenderer* deb = new DebugRenderer();
		if (deb)
		{
			deb->line_effect = line_effect;
			deb->line_worldToViewUniformId = line_worldToViewUniformId;
			deb->line_viewToScreenUniformId = line_viewToScreenUniformId;

			deb->solid_shape_effect = fill_shape_effect;
			deb->wire_localToWorldUniformId = wire_localToWorldUniformId;
			deb->wire_worldToViewUniformId = wire_worldToViewUniformId;
			deb->wire_viewToScreenUniformId = wire_viewToScreenUniformId;

			deb->wireframe_shape_effect = wireframe_shape_effect;
			deb->solid_localToWorldUniformId = solid_localToWorldUniformId;
			deb->solid_worldToViewUniformId = solid_worldToViewUniformId;
			deb->solid_viewToScreenUniformId = solid_viewToScreenUniformId;

			deb->context = i_context;
			deb->line_renderer = line_renderer;
			deb->max_lines_count = i_line_count;
			deb->line_vertices.reserve(i_line_count);
			return deb;
		}
		else
		{
			Lame::UserOutput::Display("Failed to Create DebugRenderer");
			return nullptr;
		}
	}

	bool DebugRenderer::AddLine(const Lame::Vector3& i_start, const Lame::Vector3& i_end, const Lame::Color32& i_color)
	{
		return AddLine(i_start, i_end, i_color, i_color);
	}

	bool DebugRenderer::AddLine(const Lame::Vector3& i_start, const Lame::Vector3& i_end, const Lame::Color32& i_start_color, const Lame::Color32& i_end_color)
	{
		if (line_vertices.size() >= max_lines_count)
			return false;

		Lame::Vertex start;
		start.position = i_start;
		start.color = i_start_color;
		line_vertices.push_back(start);

		Lame::Vertex end;
		end.position = i_end;
		end.color = i_end_color;
		line_vertices.push_back(end);
		return true;
	}

	bool DebugRenderer::Render(const Lame::Matrix4x4& i_worldToView, const Lame::Matrix4x4& i_viewToScreen)
	{
		const bool lines_rendered = RenderLines(i_worldToView, i_viewToScreen);
		const bool wireframe_meshes_rendered = RenderWireframeRenderableMeshes(i_worldToView, i_viewToScreen);
		const bool solid_meshes_rendered = RenderSolidRenderableMeshes(i_worldToView, i_viewToScreen);
		line_vertices.clear();
		wireframe_meshes.clear();
		solid_meshes.clear();
		return lines_rendered && wireframe_meshes_rendered && solid_meshes_rendered;
	}

	bool DebugRenderer::RenderLines(const Lame::Matrix4x4& i_worldToView, const Lame::Matrix4x4& i_viewToScreen)
	{
		if (line_vertices.size() / 2 == 0)
			return true;

		const bool lines_rendered = !line_renderer->UpdateVertices(line_vertices.data(), line_vertices.size()) ||
			!line_effect->Bind() ||
			!line_effect->SetConstant(Effect::Shader::Vertex, line_worldToViewUniformId, i_worldToView) ||
			!line_effect->SetConstant(Effect::Shader::Vertex, line_viewToScreenUniformId, i_viewToScreen) ||
			!line_renderer->Draw(line_vertices.size() / 2);
		return lines_rendered;
	}

	bool DebugRenderer::RenderSolidRenderableMeshes(const Lame::Matrix4x4& i_worldToView, const Lame::Matrix4x4& i_viewToScreen)
	{
		if (solid_meshes.size() == 0)
			return true;

		if (solid_shape_effect->Bind() &&
			solid_shape_effect->SetConstant(Lame::Effect::Shader::Vertex, solid_worldToViewUniformId, i_worldToView) &&
			solid_shape_effect->SetConstant(Lame::Effect::Shader::Vertex, solid_viewToScreenUniformId, i_viewToScreen))
		{
			bool success = true;
			for (auto itr = solid_meshes.begin(); itr != solid_meshes.end(); ++itr)
			{
				success =
					solid_shape_effect->SetConstant(Lame::Effect::Shader::Vertex, solid_localToWorldUniformId, itr->transform.LocalToWorld()) &&
					itr->mesh->Draw() &&
					success;
			}
			return success;
		}
		else
		{
			return false;
		}
	}

	bool DebugRenderer::RenderWireframeRenderableMeshes(const Lame::Matrix4x4& i_worldToView, const Lame::Matrix4x4& i_viewToScreen)
	{
		if (wireframe_meshes.size() == 0)
			return true;

		if (wireframe_shape_effect->Bind() &&
			wireframe_shape_effect->SetConstant(Lame::Effect::Shader::Vertex, wire_worldToViewUniformId, i_worldToView) &&
			wireframe_shape_effect->SetConstant(Lame::Effect::Shader::Vertex, wire_viewToScreenUniformId, i_viewToScreen))
		{
			bool success = true;
			for (auto itr = wireframe_meshes.begin(); itr != wireframe_meshes.end(); ++itr)
			{
				success =
					wireframe_shape_effect->SetConstant(Lame::Effect::Shader::Vertex, wire_localToWorldUniformId, itr->transform.LocalToWorld()) &&
					itr->mesh->Draw() &&
					success;
			}
			return success;
		}
		else
		{
			return false;
		}
	}

	bool DebugRenderer::AddMesh(const Mesh& i_mesh, const Lame::Transform& i_transform, const bool i_render_as_wireframe)
	{
		DebugRenderableMesh dm;
		dm.mesh = std::shared_ptr<Lame::RenderableMesh>(Lame::RenderableMesh::Create(true, context, i_mesh));
		if (!dm.mesh)
			return false;

		dm.transform = i_transform;
		if (i_render_as_wireframe)
			wireframe_meshes.push_back(dm);
		else
			solid_meshes.push_back(dm);
		return true;
	}

	bool DebugRenderer::AddBox(const bool i_render_wireframe, const Lame::Vector3& i_size, const Lame::Transform& i_transform, const Color32& i_color)
	{
		Mesh mesh;
		mesh.SetBox(i_size, i_color);
		return AddMesh(mesh, i_transform, i_render_wireframe);
	}

	bool DebugRenderer::AddSphere(const bool i_render_wireframe, const float i_radius, const Lame::Transform& i_transform, const Color32& i_color)
	{
		Mesh mesh; 
		mesh.SetSphere(i_radius, 10, 10, i_color);
		return AddMesh(mesh, i_transform, i_render_wireframe);
	}

	bool DebugRenderer::AddCylinder(const bool i_render_wireframe, const float i_top_radius, const float i_bottom_radius, const float i_height, const Lame::Transform& i_transform, const Color32& i_color)
	{
		Mesh mesh;
		mesh.SetCylinder(i_bottom_radius, i_top_radius, i_height, 10, 10, i_color);
		return AddMesh(mesh, i_transform, i_render_wireframe);
	}
}

#endif //ENABLE_DEBUG_RENDERING
