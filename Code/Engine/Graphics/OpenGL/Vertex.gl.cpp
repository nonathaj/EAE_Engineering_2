
#include "../Vertex.h"

#include <string>
#include <sstream>
#include "../Context.h"
#include "../../System/UserOutput.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"

namespace Lame
{
	bool Vertex::SetVertexFormat(std::shared_ptr<Lame::Context> i_context)
	{
		const GLsizei stride = sizeof(Vertex);
		struct VertexAttributes
		{
			const GLuint location;
			const GLint elementCount;
			const GLboolean normalized;
			const GLenum type;
			const size_t offset;
			const std::string identifier;
		} attributes[] = {
			{ 0, 3, GL_FALSE, GL_FLOAT, 0, "Position" },
			{ 1, 2, GL_FALSE, GL_FLOAT, 12, "Texcoord" },
			{ 2, 4, GL_TRUE, GL_UNSIGNED_BYTE, 20, "Color" },
		};
		const size_t attribute_count = 3;

		for (size_t x = 0; x < attribute_count; x++)
		{
			glVertexAttribPointer(
				attributes[x].location,
				attributes[x].elementCount,
				attributes[x].type,
				attributes[x].normalized,
				stride,
				reinterpret_cast<GLvoid*>(attributes[x].offset) );
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glEnableVertexAttribArray(attributes[x].location);
				errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the " << attributes[x].identifier << " vertex attribute: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					return false;
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to set the " << attributes[x].identifier << " vertex attribute: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				return false;
			}
		}
		return true;
	}
}
