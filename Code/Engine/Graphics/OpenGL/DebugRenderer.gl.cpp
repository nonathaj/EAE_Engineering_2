
#include "../DebugRenderer.h"

#ifdef ENABLE_DEBUG_RENDERING

#include "../Vertex.h"
#include "../Effect.h"
#include "../Context.h"
#include "../../Core/Matrix4x4.h"
#include "../../System/UserOutput.h"
#include "../../Core/Quaternion.h"
#include "../../../External/OpenGLExtensions/OpenGlExtensions.h"

namespace Lame
{
	DebugRenderer* DebugRenderer::Create(std::shared_ptr<Lame::Effect> i_effect, const size_t i_line_count)
	{
		GLuint vertex_buffer_id = 0;
		GLuint vertex_array_id = 0;

		DebugRenderer* deb = new DebugRenderer();
		if (deb)
		{
			deb->max_lines_count = i_line_count;
			deb->effect = i_effect;
			deb->line_vertices.reserve(i_line_count);
			deb->vertex_buffer_id_ = vertex_buffer_id;
			deb->vertex_array_id_ = vertex_array_id;
			return deb;
		}
		else
		{
			System::UserOutput::Display("Failed to Create DebugRenderer");
			return nullptr;
		}
	}

	bool DebugRenderer::Render(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen)
	{
		//TODO finish rendering of OpenGL lines
		//glShadeModel(GL_SMOOTH);
		effect->Bind();
		glBindVertexArray(vertex_array_id_); // setup for the layout of LineSegment_t
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Lame::Vertex) / 2 * line_vertices.size(), line_vertices.data(), GL_DYNAMIC_DRAW);
		glDrawArrays(GL_LINES, 0, line_vertices.size());
		return glGetError() != GL_NO_ERROR;
	}
}

#endif //ENABLE_DEBUG_RENDERING
