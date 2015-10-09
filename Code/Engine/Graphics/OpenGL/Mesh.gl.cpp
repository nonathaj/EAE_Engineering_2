
#include "../Mesh.h"

#include <cassert>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <string>
#include <sstream>

#include "../Vertex.h"
#include "../../System/UserOutput.h"
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"

namespace Lame
{
	Mesh::Mesh(size_t i_vertex_count, size_t i_index_count, Context *&i_context) :
		vertex_count_(i_vertex_count), 
		index_count_(i_index_count),
		context(i_context),
		vertex_array_id_(0)
	{ }

	Mesh::~Mesh()
	{
		if (vertex_array_id_ != 0)
		{
			const GLsizei arrayCount = 1;
			glDeleteVertexArrays(arrayCount, &vertex_array_id_);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the vertex array: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
			}
			vertex_array_id_ = 0;
		}
	}

	//Create a mesh with RIGHT-HANDED indices
	Mesh* Mesh::Create(Context *&i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count)
	{
		if (i_index_count % 3 != 0)		//index buffer must be a list of triangles
		{
			System::UserOutput::Display("Cannot create a Mesh with non-triangular data. (Ensure number of indices is divisible by 3)");
			return nullptr;
		}

		bool wereThereErrors = false;
		GLuint vertexBufferId = 0;
		GLuint indexBufferId = 0;

		GLuint vertex_array_id_ = 0;

		// Create a vertex array object and make it active
		{
			const GLsizei arrayCount = 1;
			glGenVertexArrays(arrayCount, &vertex_array_id_);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindVertexArray(vertex_array_id_);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the vertex array: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex array ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				goto OnExit;
			}
		}

		// Create a vertex buffer object and make it active
		{
			const GLsizei bufferCount = 1;
			glGenBuffers(bufferCount, &vertexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the vertex buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused vertex buffer ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				goto OnExit;
			}
		}
		// Assign the data to the buffer
		{
			glBufferData(GL_ARRAY_BUFFER, i_vertex_count * sizeof(Vertex), reinterpret_cast<GLvoid*>(i_vertices),
				// Our code will only ever write to the buffer
				GL_STATIC_DRAW);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to allocate the vertex buffer: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				goto OnExit;
			}
		}
		// Initialize the vertex format
		{
			const GLsizei stride = sizeof(Vertex);
			GLvoid* offset = 0;

			// Position (0)
			// 2 floats == 8 bytes
			// Offset = 0
			{
				const GLuint vertexElementLocation = 0;
				const GLint elementCount = 2;
				const GLboolean notNormalized = GL_FALSE;	// The given floats should be used as-is
				glVertexAttribPointer(vertexElementLocation, elementCount, GL_FLOAT, notNormalized, stride, offset);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					glEnableVertexAttribArray(vertexElementLocation);
					const GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						offset = reinterpret_cast<GLvoid*>(reinterpret_cast<uint8_t*>(offset) + (elementCount * sizeof(float)));
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to enable the POSITION vertex attribute: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the POSITION vertex attribute: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					goto OnExit;
				}
			}
			// Color (1)
			// 4 uint8_ts == 4 bytes
			// Offset = 8
			{
				const GLuint vertexElementLocation = 1;
				const GLint elementCount = 4;
				// Each element will be sent to the GPU as an unsigned byte in the range [0,255]
				// but these values should be understood as representing [0,1] values
				// and that is what the shader code will interpret them as
				// (in other words, we could change the values provided here in C code
				// to be floats and sent GL_FALSE instead and the shader code wouldn't need to change)
				const GLboolean normalized = GL_TRUE;
				glVertexAttribPointer(vertexElementLocation, elementCount, GL_UNSIGNED_BYTE, normalized, stride, offset);
				const GLenum errorCode = glGetError();
				if (errorCode == GL_NO_ERROR)
				{
					glEnableVertexAttribArray(vertexElementLocation);
					const GLenum errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						offset = reinterpret_cast<GLvoid*>(reinterpret_cast<uint8_t*>(offset) + (elementCount * sizeof(uint8_t)));
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to enable the COLOR0 vertex attribute: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the COLOR0 vertex attribute: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					goto OnExit;
				}
			}
		}

		// Create an index buffer object and make it active
		{
			const GLsizei bufferCount = 1;
			glGenBuffers(bufferCount, &indexBufferId);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to bind the index buffer: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to get an unused index buffer ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				goto OnExit;
			}
		}
		// Allocate space and copy the triangle data into the index buffer
		{
			const GLsizeiptr bufferSize = i_index_count * sizeof(uint32_t);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize, reinterpret_cast<const GLvoid*>(i_indices),
				// Our code will only ever write to the buffer
				GL_STATIC_DRAW);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to allocate the index buffer: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				goto OnExit;
			}
		}

	OnExit:

		// Delete the buffer object
		// (the vertex array will hold a reference to it)
		if (vertex_array_id_ != 0)
		{
			// Unbind the vertex array
			// (this must be done before deleting the vertex buffer)
			glBindVertexArray(0);
			const GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				if (vertexBufferId != 0)
				{
					// NOTE: If you delete the vertex buffer object here, as I recommend,
					// then gDEBugger won't know about it and you won't be able to examine the data.
					// If you find yourself in a situation where you want to see the buffer object data in gDEBugger
					// comment out this block of code temporarily
					// (doing this will cause a memory leak so make sure to restore the deletion code after you're done debugging).
					const GLsizei bufferCount = 1;
					glDeleteBuffers(bufferCount, &vertexBufferId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to delete the vertex buffer: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
					}
					vertexBufferId = 0;
				}
				if (indexBufferId != 0)
				{
					// NOTE: See the same comment above about deleting the vertex buffer object here and gDEBugger
					// holds true for the index buffer
					const GLsizei bufferCount = 1;
					glDeleteBuffers(bufferCount, &indexBufferId);
					const GLenum errorCode = glGetError();
					if (errorCode != GL_NO_ERROR)
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "\nOpenGL failed to delete the index buffer: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
					}
					indexBufferId = 0;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to unbind the vertex array: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
			}
		}

		if (!wereThereErrors)
		{
			Mesh *mesh = new Mesh(i_vertex_count, i_index_count, i_context);
			if (mesh)
			{
				mesh->vertex_array_id_ = vertex_array_id_;
				return mesh;
			}
			else
				return nullptr;
		}
		else
			return nullptr;
	}

	bool Mesh::Draw()
	{
		// Bind a specific vertex buffer to the device as a data source
		{
			glBindVertexArray(vertex_array_id_);
			if (glGetError() != GL_NO_ERROR)
			{
				System::UserOutput::Display("Failed to bind vertex array");
				return false;
			}
		}
		// Render objects from the current streams
		{
			// We are using triangles as the "primitive" type,
			// and we have defined the vertex buffer as a triangle list
			// (meaning that every triangle is defined by three vertices)
			const GLenum mode = GL_TRIANGLES;
			// We'll use 32-bit indices in this class to keep things simple
			// (i.e. every index will be a 32 bit unsigned integer)
			const GLenum indexType = GL_UNSIGNED_INT;
			// It is possible to start rendering in the middle of an index buffer
			const GLvoid* const offset = 0;
			glDrawElements(mode, index_count_, indexType, offset);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				return true;
			}
			else
			{
				System::UserOutput::Display("Error while attempting to draw mesh: %s", reinterpret_cast<const char *>(gluErrorString(errorCode)));
				return false;
			}
		}
	}
}
