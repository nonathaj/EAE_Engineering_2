
#include "../DebugRenderer.h"

#include <sstream>
#include <string>
#include <gl/GL.h>
#include <gl/GLU.h>

#ifdef ENABLE_DEBUG_RENDERING

#include "../Vertex.h"
#include "../Effect.h"
#include "../Context.h"
#include "../../Core/Matrix4x4.h"
#include "../../System/UserOutput.h"
#include "../../Core/Quaternion.h"
#include "../../../External/OpenGLExtensions/OpenGlExtensions.h"

namespace
{
	bool Cleanup(GLuint vertex_array_id, GLuint vertex_buffer_id)
	{
		// Delete the buffer object
		// (the vertex array will hold a reference to it)
		if (vertex_array_id != 0)
		{
			// Unbind the vertex array
			// (this must be done before deleting the vertex buffer)
			glBindVertexArray(0);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				if (vertex_buffer_id != 0)
				{
					// NOTE: If you delete the vertex buffer object here, as I recommend,
					// then gDEBugger won't know about it and you won't be able to examine the data.
					// If you find yourself in a situation where you want to see the buffer object data in gDEBugger
					// comment out this block of code temporarily
					// (doing this will cause a memory leak so make sure to restore the deletion code after you're done debugging).
					const GLsizei bufferCount = 1;
					glDeleteBuffers(bufferCount, &vertex_buffer_id);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to delete the vertex buffer: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
						return false;
					}
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to unbind the vertex array: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				return false;
			}
		}
		return true;
	}
}

namespace Lame
{
	DebugRenderer* DebugRenderer::Create(std::shared_ptr<Lame::Context> i_context, const size_t i_line_count)
	{
		GLuint vertex_buffer_id = 0;
		GLuint vertex_array_id = 0;

		std::shared_ptr<Lame::Effect> line_effect(Lame::Effect::Create(i_context, "data/debug/line.effect.bin", false));
		std::shared_ptr<Lame::Effect> shape_effect(Lame::Effect::Create(i_context, "data/debug/shape.effect.bin", true));
		if (!line_effect || !shape_effect)
		{
			System::UserOutput::Display("Failed to Create Debug Effects");
			return nullptr;
		}

		// Create a vertex array object and make it active
		{
			const GLsizei arrayCount = 1;
			glGenVertexArrays(arrayCount, &vertex_array_id);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindVertexArray(vertex_array_id);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the vertex array: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					return nullptr;
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex array ID: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				return nullptr;
			}
		}

		// Create a vertex buffer object and make it active
		{
			const GLsizei bufferCount = 1;
			glGenBuffers(bufferCount, &vertex_buffer_id);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the vertex buffer: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					Cleanup(vertex_array_id, vertex_buffer_id);
					return nullptr;
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex buffer ID: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				Cleanup(vertex_array_id, vertex_buffer_id);
				return nullptr;
			}
		}

		DebugRenderer* deb = new DebugRenderer();
		if (deb)
		{
			deb->max_lines_count = i_line_count;
			deb->line_effect = line_effect;
			deb->shape_effect = shape_effect;
			deb->line_vertices.reserve(i_line_count);
			deb->vertex_array_id_ = vertex_array_id;
			deb->vertex_buffer_id_ = vertex_buffer_id;
			return deb;
		}
		else
		{
			System::UserOutput::Display("Failed to Create DebugRenderer");
			Cleanup(vertex_array_id, vertex_buffer_id);
			return nullptr;
		}
	}

	DebugRenderer::~DebugRenderer()
	{
		Cleanup(vertex_array_id_, vertex_buffer_id_);
	}

	bool DebugRenderer::RenderLines(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen)
	{
		//TODO finish rendering of OpenGL lines
		//glShadeModel(GL_SMOOTH);
		//set the effect
		if (!line_effect->Bind() ||
			!line_effect->SetWorldToView(i_worldToView) ||
			!line_effect->SetViewToScreen(i_viewToScreen))
		{
			return false;
		}

		glBindVertexArray(vertex_array_id_);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			System::UserOutput::Display("OpenGL failed to bind vertex array");
			return false;
		}

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id_);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			System::UserOutput::Display("OpenGL failed to bind vertex buffer");
			return false;
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(Lame::Vertex) * line_vertices.size() / 2, line_vertices.data(), GL_DYNAMIC_DRAW);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			System::UserOutput::Display("OpenGL failed to send data to vertex buffer");
			return false;
		}

		glDrawArrays(GL_LINES, 0, line_vertices.size());
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			System::UserOutput::Display("OpenGL failed to draw lines");
			return false;
		}

		return glGetError() != GL_NO_ERROR;
	}
}

#endif //ENABLE_DEBUG_RENDERING
