
#include "../DebugRenderer.h"

#ifdef ENABLE_DEBUG_RENDERING

#include "../Vertex.h"
#include "../Effect.h"
#include "../Context.h"
#include "../../Core/Matrix4x4.h"
#include "../../System/UserOutput.h"
#include "../../Core/Quaternion.h"

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
}

#endif //ENABLE_DEBUG_RENDERING
