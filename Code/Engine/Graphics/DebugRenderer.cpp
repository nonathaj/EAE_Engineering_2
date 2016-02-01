
#include "DebugRenderer.h"

#ifdef ENABLE_DEBUG_RENDERING

#include "Vertex.h"
#include "Effect.h"
#include "Context.h"
#include "Mesh.h"
#include "../Core/Matrix4x4.h"
#include "../System/UserOutput.h"
#include "../Core/Quaternion.h"
#include "../System/Console.h"

namespace Lame
{
	char const * const DebugRenderer::LocalToWorldUniformName = "local_to_world";
	char const * const DebugRenderer::WorldToViewUniformName = "world_to_view";
	char const * const DebugRenderer::ViewToScreenUniformName = "view_to_screen";

	bool DebugRenderer::AddLine(const Engine::Vector3& i_start, const Engine::Vector3& i_end, const Lame::Color32& i_color)
	{
		return AddLine(i_start, i_end, i_color, i_color);
	}

	bool DebugRenderer::AddLine(const Engine::Vector3& i_start, const Engine::Vector3& i_end, const Lame::Color32& i_start_color, const Lame::Color32& i_end_color)
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

	bool DebugRenderer::AddMesh(std::shared_ptr<Lame::Mesh> i_mesh, const Engine::Transform& i_transform, const bool i_render_as_wireframe)
	{
		if (!i_mesh)
			return false;

		DebugMesh dm;
		dm.mesh = i_mesh;
		dm.transform = i_transform;
		if (i_render_as_wireframe)
			wireframe_meshes.push_back(dm);
		else
			solid_meshes.push_back(dm);
		return true;
	}

	bool DebugRenderer::Render(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen)
	{
		const bool lines_rendered = RenderLines(i_worldToView, i_viewToScreen);
		const bool wireframe_meshes_rendered = RenderWireframeMeshes(i_worldToView, i_viewToScreen);
		const bool solid_meshes_rendered = RenderSolidMeshes(i_worldToView, i_viewToScreen);
		line_vertices.clear();
		wireframe_meshes.clear();
		solid_meshes.clear();
		return lines_rendered && wireframe_meshes_rendered && solid_meshes_rendered;
	}

	bool DebugRenderer::RenderSolidMeshes(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen)
	{
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

	bool DebugRenderer::RenderWireframeMeshes(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen)
	{
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

	bool DebugRenderer::AddBox(const bool i_render_wireframe, const Engine::Vector3& i_size, const Engine::Transform& i_transform, const Color32& i_color)
	{
		std::shared_ptr<Lame::Mesh> mesh(Lame::Mesh::CreateBox(solid_shape_effect->get_context(), i_size, i_color));
		return AddMesh(mesh, i_transform, i_render_wireframe);
	}

	bool DebugRenderer::AddSphere(const bool i_render_wireframe, const float i_radius, const Engine::Transform& i_transform, const Color32& i_color)
	{
		std::shared_ptr<Lame::Mesh> mesh(Lame::Mesh::CreateSphere(solid_shape_effect->get_context(), i_radius, 10, 10, i_color));
		return AddMesh(mesh, i_transform, i_render_wireframe);
	}

	bool DebugRenderer::AddCylinder(const bool i_render_wireframe, const float i_top_radius, const float i_bottom_radius, const float i_height, const Engine::Transform& i_transform, const Color32& i_color)
	{
		std::shared_ptr<Lame::Mesh> mesh(Lame::Mesh::CreateCylinder(solid_shape_effect->get_context(), i_bottom_radius, i_top_radius, i_height, 10, 10, i_color));
		return AddMesh(mesh, i_transform, i_render_wireframe);
	}
}

#endif //ENABLE_DEBUG_RENDERING
