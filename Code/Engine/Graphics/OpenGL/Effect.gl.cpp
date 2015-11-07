
#include "../Effect.h"

#include "../../Windows/Includes.h"
#include "../../Windows/Functions.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <string>
#include <sstream>

#include "../../System/UserOutput.h"
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"

namespace
{
	bool LoadAndAllocateShaderProgram(const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage);
	bool LoadFragmentShader(std::string i_path, GLuint i_programId);
	bool LoadVertexShader(std::string i_path, const GLuint i_programId);
	bool CreateProgram(GLuint &o_programId, std::string i_vertex_path, std::string i_fragment_path);

	// This helper struct exists to be able to dynamically allocate memory to get "log info"
	// which will automatically be freed when the struct goes out of scope
	struct sLogInfo
	{
		GLchar* memory;
		sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
		~sLogInfo() { if (memory) free(memory); }
	};
}

namespace Lame
{
	Effect* Effect::Create(std::shared_ptr<Context> i_context, const char* i_vertex_path, const char* i_fragment_path)
	{
		// A vertex shader is a program that operates on vertices.
		// Its input comes from a C/C++ "draw call" and is:
		//	* Position
		//	* Any other data we want
		// Its output is:
		//	* Position
		//		(So that the graphics hardware knows which pixels to fill in for the triangle)
		//	* Any other data we want

		// The fragment shader is a program that operates on fragments
		// (or potential pixels).
		// Its input is:
		//	* The data that was output from the vertex shader,
		//		interpolated based on how close the fragment is
		//		to each vertex in the triangle.
		// Its output is:
		//	* The final color that the pixel should be
		GLuint programId;
		if (!CreateProgram(programId, i_vertex_path, i_fragment_path))
			return nullptr;
		
		Effect *effect = new Effect(i_context);
		if (effect)
		{
			effect->programId = programId;

			//find the uniform handles
			if (!effect->CacheConstant(LocalToWorldUniformName) ||
				!effect->CacheConstant(WorldToViewUniformName) ||
				!effect->CacheConstant(ViewToScreenUniformName))
			{
				System::UserOutput::Display("OpenGL failed to find all required uniform constants for effect.");
				delete effect;
				return nullptr;
			}
		}
		else
		{
			System::UserOutput::Display("Failed to create Effect, due to insufficient memory.", "Effect Loading Error");
		}
		return effect;
	}

	bool Effect::Bind()
	{
		// Set the vertex and fragment shaders
		glUseProgram(programId);
		assert(glGetError() == GL_NO_ERROR);
		return true;
	}

	Effect::~Effect()
	{
		if (programId != 0)
		{
			glDeleteProgram(programId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
			}
			programId = 0;
		}
	}

	bool Effect::CacheConstant(const std::string &i_constant, Engine::HashedString* o_constantId)
	{
		Engine::HashedString hashed(i_constant.c_str());
		if (o_constantId)
			*o_constantId = hashed;

		//if we already have this constant cached, we already successfully cached it
		if (constants.find(hashed) != constants.end())
			return true;

		GLint location = glGetUniformLocation(programId, i_constant.c_str());
		if (location >= 0)
			constants[hashed] = location;
		return location >= 0;
	}

	bool Effect::SetConstant(const Engine::HashedString &i_constant, const eae6320::Math::cVector &i_val)
	{
		auto itr = constants.find(i_constant);
		if (itr == constants.end())					//fail if we don't have a cache'd version of this constant
			return false;

		GLint handle = itr->second;
		glUniform3fv(handle, 1, reinterpret_cast<const float*>(&i_val));

		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to set a constant uniform value: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
			System::UserOutput::Display(errorMessage.str());
			return false;
		}
		return true;
	}


	bool Effect::SetConstant(const Engine::HashedString &i_constant, const eae6320::Math::cMatrix_transformation &i_val)
	{
		auto itr = constants.find(i_constant);
		if (itr == constants.end())					//fail if we don't have a cache'd version of this constant
			return false;

		GLint handle = itr->second;
		const GLboolean shouldTranspose = false; // Matrices are already in the correct format
		const GLsizei uniformCountToSet = 1;
		glUniformMatrix4fv(handle, uniformCountToSet, shouldTranspose, reinterpret_cast<const GLfloat*>(&i_val));

		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to set a constant uniform value: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
			System::UserOutput::Display(errorMessage.str());
			return false;
		}
		return true;
	}
}

namespace
{
	bool CreateProgram(GLuint &o_programId, std::string i_vertex_path, std::string i_fragment_path)
	{
		// Create a program
		{
			o_programId = glCreateProgram();
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to create a program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				return false;
			}
			else if (o_programId == 0)
			{
				System::UserOutput::Display("OpenGL failed to create a program");
				return false;
			}
		}
		// Load and attach the shaders
		if (!LoadVertexShader(i_vertex_path, o_programId))
		{
			return false;
		}
		if (!LoadFragmentShader(i_fragment_path, o_programId))
		{
			return false;
		}
		// Link the program
		{
			glLinkProgram(o_programId);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// Get link info
				// (this won't be used unless linking fails
				// but it can be useful to look at when debugging)
				std::string linkInfo;
				{
					GLint infoSize;
					glGetProgramiv(o_programId, GL_INFO_LOG_LENGTH, &infoSize);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						sLogInfo info(static_cast<size_t>(infoSize));
						GLsizei* dontReturnLength = NULL;
						glGetProgramInfoLog(o_programId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							linkInfo = info.memory;
						}
						else
						{
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get link info of the program: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							System::UserOutput::Display(errorMessage.str());
							return false;
						}
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the program link info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
						return false;
					}
				}
				// Check to see if there were link errors
				GLint didLinkingSucceed;
				{
					glGetProgramiv(o_programId, GL_LINK_STATUS, &didLinkingSucceed);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						if (didLinkingSucceed == GL_FALSE)
						{
							std::stringstream errorMessage;
							errorMessage << "The program failed to link:\n" << linkInfo;
							System::UserOutput::Display(errorMessage.str());
							return false;
						}
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if linking of the program succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
						return false;
					}
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to link the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				return false;
			}
		}

		return true;
	}

	bool LoadAndAllocateShaderProgram(const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage)
	{
		bool wereThereErrors = false;

		// Load the shader source from disk
		o_shader = NULL;
		HANDLE fileHandle = INVALID_HANDLE_VALUE;
		{
			// Open the file
			{
				const DWORD desiredAccess = FILE_GENERIC_READ;
				const DWORD otherProgramsCanStillReadTheFile = FILE_SHARE_READ;
				SECURITY_ATTRIBUTES* useDefaultSecurity = NULL;
				const DWORD onlySucceedIfFileExists = OPEN_EXISTING;
				const DWORD useDefaultAttributes = FILE_ATTRIBUTE_NORMAL;
				const HANDLE dontUseTemplateFile = NULL;
				fileHandle = CreateFile(i_path, desiredAccess, otherProgramsCanStillReadTheFile,
					useDefaultSecurity, onlySucceedIfFileExists, useDefaultAttributes, dontUseTemplateFile);
				if (fileHandle == INVALID_HANDLE_VALUE)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage = eae6320::GetLastWindowsError();
						std::stringstream errorMessage;
						errorMessage << "Windows failed to open the shader file: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			// Get the file's size
			{
				LARGE_INTEGER fileSize_integer;
				if (GetFileSizeEx(fileHandle, &fileSize_integer) != FALSE)
				{
					assert(fileSize_integer.QuadPart <= SIZE_MAX);
					o_size = static_cast<size_t>(fileSize_integer.QuadPart);
				}
				else
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage = eae6320::GetLastWindowsError();
						std::stringstream errorMessage;
						errorMessage << "Windows failed to get the size of shader: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
				// Add an extra byte for a NULL terminator
				o_size += 1;
			}
			// Read the file's contents into temporary memory
			o_shader = malloc(o_size);
			if (o_shader)
			{
				DWORD bytesReadCount;
				OVERLAPPED* readSynchronously = NULL;
				if (ReadFile(fileHandle, o_shader, o_size,
					&bytesReadCount, readSynchronously) == FALSE)
				{
					wereThereErrors = true;
					if (o_errorMessage)
					{
						std::string windowsErrorMessage = eae6320::GetLastWindowsError();
						std::stringstream errorMessage;
						errorMessage << "Windows failed to read the contents of shader: " << windowsErrorMessage;
						*o_errorMessage = errorMessage.str();
					}
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				if (o_errorMessage)
				{
					std::stringstream errorMessage;
					errorMessage << "Failed to allocate " << o_size << " bytes to read in the shader program " << i_path;
					*o_errorMessage = errorMessage.str();
				}
				goto OnExit;
			}
			// Add the NULL terminator
			reinterpret_cast<char*>(o_shader)[o_size - 1] = '\0';
		}

	OnExit:

		if (wereThereErrors && o_shader)
		{
			free(o_shader);
			o_shader = NULL;
		}
		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			if (CloseHandle(fileHandle) == FALSE)
			{
				if (!wereThereErrors && o_errorMessage)
				{
					std::string windowsError = eae6320::GetLastWindowsError();
					std::stringstream errorMessage;
					errorMessage << "Windows failed to close the shader file handle: " << windowsError;
					*o_errorMessage = errorMessage.str();
				}
				wereThereErrors = true;
			}
			fileHandle = INVALID_HANDLE_VALUE;
		}

		return !wereThereErrors;
	}

	bool LoadFragmentShader(std::string i_path, GLuint i_programId)
	{
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
			if (!isShaderCompilingSupported)
			{
				System::UserOutput::Display("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
				return false;
			}
		}

		bool wereThereErrors = false;

		// Load the source code from file and set it into a shader
		GLuint fragmentShaderId = 0;
		void* shaderSource = NULL;
		{
			// Load the shader source code
			size_t fileSize;
			{
				std::string errorMessage;
				if (!LoadAndAllocateShaderProgram(i_path.c_str(), shaderSource, fileSize, &errorMessage))
				{
					wereThereErrors = true;
					System::UserOutput::Display(errorMessage);
					goto OnExit;
				}
			}
			// Generate a shader
			fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
			{
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get an unused fragment shader ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					goto OnExit;
				}
				else if (fragmentShaderId == 0)
				{
					wereThereErrors = true;
					System::UserOutput::Display("OpenGL failed to get an unused fragment shader ID");
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 1;
				const GLchar* shaderSources[] =
				{
					reinterpret_cast<GLchar*>(shaderSource)
				};
				const GLint* sourcesAreNullTerminated = NULL;
				const GLint length = static_cast<GLuint>(fileSize);
				glShaderSource(fragmentShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the fragment shader source code: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					goto OnExit;
				}
			}
		}
		// Compile the shader source code
		{
			glCompileShader(fragmentShaderId);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// Get compilation info
				// (this won't be used unless compilation fails
				// but it can be useful to look at when debugging)
				std::string compilationInfo;
				{
					GLint infoSize;
					glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						sLogInfo info(static_cast<size_t>(infoSize));
						GLsizei* dontReturnLength = NULL;
						glGetShaderInfoLog(fragmentShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							compilationInfo = info.memory;
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get compilation info of the fragment shader source code: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							System::UserOutput::Display(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the fragment shader compilation info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
						goto OnExit;
					}
				}
				// Check to see if there were compilation errors
				GLint didCompilationSucceed;
				{
					glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						if (didCompilationSucceed == GL_FALSE)
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "The fragment shader failed to compile:\n" << compilationInfo;
							System::UserOutput::Display(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the fragment shader source code succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to compile the fragment shader source code: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				goto OnExit;
			}
		}
		// Attach the shader to the program
		{
			glAttachShader(i_programId, fragmentShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to attach the fragment shader to the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				goto OnExit;
			}
		}

	OnExit:

		if (fragmentShaderId != 0)
		{
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			glDeleteShader(fragmentShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the fragment shader ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
			}
			fragmentShaderId = 0;
		}
		if (shaderSource != NULL)
		{
			free(shaderSource);
			shaderSource = NULL;
		}

		return !wereThereErrors;
	}

	bool LoadVertexShader(std::string i_path, GLuint i_programId)
	{
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
			if (!isShaderCompilingSupported)
			{
				System::UserOutput::Display("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
				return false;
			}
		}

		bool wereThereErrors = false;

		// Load the source code from file and set it into a shader
		GLuint vertexShaderId = 0;
		void* shaderSource = NULL;
		{
			// Load the shader source code
			size_t fileSize;
			{
				std::string errorMessage;
				if (!LoadAndAllocateShaderProgram(i_path.c_str(), shaderSource, fileSize, &errorMessage))
				{
					wereThereErrors = true;
					System::UserOutput::Display(errorMessage);
					goto OnExit;
				}
			}
			// Generate a shader
			vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
			{
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get an unused vertex shader ID: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					goto OnExit;
				}
				else if (vertexShaderId == 0)
				{
					wereThereErrors = true;
					System::UserOutput::Display("OpenGL failed to get an unused vertex shader ID");
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 1;
				const GLchar* shaderSources[] =
				{
					reinterpret_cast<GLchar*>(shaderSource)
				};
				const GLint* sourcesAreNullTerminated = NULL;
				glShaderSource(vertexShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
				const GLenum errorCode = glGetError();
				if (errorCode != GL_NO_ERROR)
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the vertex shader source code: " <<
						reinterpret_cast<const char*>(gluErrorString(errorCode));
					System::UserOutput::Display(errorMessage.str());
					goto OnExit;
				}
			}
		}
		// Compile the shader source code
		{
			glCompileShader(vertexShaderId);
			GLenum errorCode = glGetError();
			if (errorCode == GL_NO_ERROR)
			{
				// Get compilation info
				// (this won't be used unless compilation fails
				// but it can be useful to look at when debugging)
				std::string compilationInfo;
				{
					GLint infoSize;
					glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &infoSize);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						sLogInfo info(static_cast<size_t>(infoSize));
						GLsizei* dontReturnLength = NULL;
						glGetShaderInfoLog(vertexShaderId, static_cast<GLsizei>(infoSize), dontReturnLength, info.memory);
						errorCode = glGetError();
						if (errorCode == GL_NO_ERROR)
						{
							compilationInfo = info.memory;
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get compilation info of the vertex shader source code: " <<
								reinterpret_cast<const char*>(gluErrorString(errorCode));
							System::UserOutput::Display(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the vertex shader compilation info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
						goto OnExit;
					}
				}
				// Check to see if there were compilation errors
				GLint didCompilationSucceed;
				{
					glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &didCompilationSucceed);
					errorCode = glGetError();
					if (errorCode == GL_NO_ERROR)
					{
						if (didCompilationSucceed == GL_FALSE)
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "The vertex shader failed to compile:\n" << compilationInfo;
							System::UserOutput::Display(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the vertex shader source code succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						System::UserOutput::Display(errorMessage.str());
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to compile the vertex shader source code: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				goto OnExit;
			}
		}
		// Attach the shader to the program
		{
			glAttachShader(i_programId, vertexShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to attach the vertex shader to the program: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
				goto OnExit;
			}
		}

	OnExit:

		if (vertexShaderId != 0)
		{
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			glDeleteShader(vertexShaderId);
			const GLenum errorCode = glGetError();
			if (errorCode != GL_NO_ERROR)
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the vertex shader ID: " <<
					reinterpret_cast<const char*>(gluErrorString(errorCode));
				System::UserOutput::Display(errorMessage.str());
			}
			vertexShaderId = 0;
		}
		if (shaderSource != NULL)
		{
			free(shaderSource);
			shaderSource = NULL;
		}

		return !wereThereErrors;
	}
}
