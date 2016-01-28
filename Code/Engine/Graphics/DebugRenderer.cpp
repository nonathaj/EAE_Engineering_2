
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
	bool DebugRenderer::AddLineBox(const Engine::Vector3& i_center, const Engine::Vector3& i_extends, const Lame::Color32& i_color)
	{
		if (line_vertices.size() + 12 >= max_lines_count)
			return false;

		const Engine::Vector3 half = i_extends / 2.0f;
		const Engine::Vector3 frontTopLeft = i_center + Engine::Vector3(-half.x(), half.y(), half.z());
		const Engine::Vector3 frontTopRight = i_center + Engine::Vector3(half.x(), half.y(), half.z());
		const Engine::Vector3 frontBottomLeft = i_center + Engine::Vector3(-half.x(), -half.y(), half.z());
		const Engine::Vector3 frontBottomRight = i_center + Engine::Vector3(half.x(), -half.y(), half.z());
		const Engine::Vector3 backTopLeft = i_center + Engine::Vector3(-half.x(), half.y(), -half.z());
		const Engine::Vector3 backTopRight = i_center + Engine::Vector3(half.x(), half.y(), -half.z());
		const Engine::Vector3 backBottomLeft = i_center + Engine::Vector3(-half.x(), -half.y(), -half.z());
		const Engine::Vector3 backBottomRight = i_center + Engine::Vector3(half.x(), -half.y(), -half.z());

		return
			AddLine(frontTopLeft, frontTopRight, i_color) &&
			AddLine(frontTopLeft, frontBottomLeft, i_color) &&
			AddLine(frontTopRight, frontBottomRight, i_color) &&
			AddLine(frontBottomLeft, frontBottomRight, i_color) &&
			AddLine(frontTopLeft, backTopLeft, i_color) &&
			AddLine(frontTopRight, backTopRight, i_color) &&
			AddLine(frontBottomLeft, backBottomLeft, i_color) &&
			AddLine(frontBottomRight, backBottomRight, i_color) &&
			AddLine(backTopLeft, backTopRight, i_color) &&
			AddLine(backTopLeft, backBottomLeft, i_color) &&
			AddLine(backTopRight, backBottomRight, i_color) &&
			AddLine(backBottomLeft, backBottomRight, i_color);
	}

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

	bool DebugRenderer::AddMesh(std::shared_ptr<Lame::Mesh> i_mesh, const Engine::Transform& i_transform)
	{
		if (!i_mesh)
			return false;

		DebugMesh dm;
		dm.mesh = i_mesh;
		dm.transform = i_transform;
		meshes.push_back(dm);
		return true;
	}

	bool DebugRenderer::Render(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen)
	{
		bool linesRendered = RenderLines(i_worldToView, i_viewToScreen);
		bool meshesRendered = RenderMeshes(i_worldToView, i_viewToScreen);
		line_vertices.clear();
		meshes.clear();
		return linesRendered && meshesRendered;
	}

	bool DebugRenderer::RenderMeshes(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen)
	{
		if (shape_effect->Bind() &&
			shape_effect->SetWorldToView(i_worldToView) &&
			shape_effect->SetViewToScreen(i_viewToScreen))
		{
			bool success = true;
			for (auto itr = meshes.begin(); itr != meshes.end(); ++itr)
			{
				success = 
					shape_effect->SetLocalToWorld(itr->transform.LocalToWorld()) && 
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

	bool DebugRenderer::AddFillBox(const Engine::Vector3& i_size, const Engine::Transform& i_transform, const Color32& i_color)
	{
		std::shared_ptr<Lame::Mesh> mesh(Lame::Mesh::CreateBox(shape_effect->get_context(), i_size, i_color));
		return AddMesh(mesh, i_transform);
	}

	bool DebugRenderer::AddFillSphere(const float i_radius, const Engine::Transform& i_transform, const Color32& i_color)
	{
		std::shared_ptr<Lame::Mesh> mesh(Lame::Mesh::CreateSphere(shape_effect->get_context(), i_radius, 10, 10, i_color));
		return AddMesh(mesh, i_transform);
	}

	bool DebugRenderer::AddFillCylinder(const float i_top_radius, const float i_bottom_radius, const float i_height, const Engine::Transform& i_transform, const Color32& i_color)
	{
		std::shared_ptr<Lame::Mesh> mesh(Lame::Mesh::CreateCylinder(shape_effect->get_context(), i_bottom_radius, i_top_radius, i_height, 10, 10, i_color));
		return AddMesh(mesh, i_transform);
	}
}

#endif //ENABLE_DEBUG_RENDERING
