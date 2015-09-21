// Header Files
//=============

#include "../Graphics.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <string>
#include <sstream>
#include "../../System/Console.h"
#include "../../Windows/Functions.h"
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"

#include "../Mesh.h"
#include "../Vertex.h"

// Static Data Initialization
//===========================

namespace
{
	HWND s_renderingWindow = NULL;
	HDC s_deviceContext = NULL;
	HGLRC s_openGlRenderingContext = NULL;

	// OpenGL encapsulates a matching vertex shader and fragment shader into what it calls a "program".

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
	GLuint s_programId = 0;

	Lame::Mesh *mesh = nullptr;
}

// Helper Function Declarations
//=============================

namespace
{
	bool CreateProgram();
	bool CreateRenderingContext();
	bool LoadAndAllocateShaderProgram( const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage );
	bool LoadFragmentShader( const GLuint i_programId );
	bool LoadVertexShader( const GLuint i_programId );

	// This helper struct exists to be able to dynamically allocate memory to get "log info"
	// which will automatically be freed when the struct goes out of scope
	struct sLogInfo
	{
		GLchar* memory;
		sLogInfo( const size_t i_size ) { memory = reinterpret_cast<GLchar*>( malloc( i_size ) ); }
		~sLogInfo() { if ( memory ) free( memory ); }
	};
}

// Interface
//==========

bool eae6320::Graphics::Initialize( const HWND i_renderingWindow )
{
	s_renderingWindow = i_renderingWindow;

	// Create an OpenGL rendering context
	if ( !CreateRenderingContext() )
	{
		goto OnError;
	}

	// Load any required OpenGL extensions
	{
		std::string errorMessage;
		if ( !OpenGlExtensions::Load( &errorMessage ) )
		{
			DEBUG_PRINT( errorMessage );
			goto OnError;
		}
	}

	// Initialize the graphics objects
	if ( !(mesh = Lame::Mesh::Create("data/square.mesh")) )
	{
		MessageBox(i_renderingWindow, "Failed to load the square.mesh file.", "Mesh loading error", 0);
		goto OnError;
	}

	if ( !CreateProgram() )
	{
		goto OnError;
	}

	return true;

OnError:

	ShutDown();
	return false;
}

void eae6320::Graphics::Render()
{
	// Every frame an entirely new image will be created.
	// Before drawing anything, then, the previous image will be erased
	// by "clearing" the image buffer (filling it with a solid color)
	{
		// Black is usually used
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		assert( glGetError() == GL_NO_ERROR );
		// In addition to the color, "depth" and "stencil" can also be cleared,
		// but for now we only care about color
		const GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
		glClear( clearColor );
		assert( glGetError() == GL_NO_ERROR );
	}

	// The actual function calls that draw geometry
	{
		// Set the vertex and fragment shaders
		{
			glUseProgram( s_programId );
			assert( glGetError() == GL_NO_ERROR );
		}
		bool drawSuccess = mesh->Draw();
		assert( drawSuccess );
	}

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it, the contents of the back buffer must be swapped with the "front buffer"
	// (which is what the user sees)
	{
		BOOL result = SwapBuffers( s_deviceContext );
		assert( result != FALSE );
	}
}

bool eae6320::Graphics::ShutDown()
{
	bool wereThereErrors = false;

	if ( s_openGlRenderingContext != NULL )
	{
		if ( s_programId != 0 )
		{
			glDeleteProgram( s_programId );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the program: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				DEBUG_PRINT( errorMessage.str() );
			}
			s_programId = 0;
		}

		if (mesh)
		{
			delete mesh;
			mesh = nullptr;
		}

		if ( wglMakeCurrent( s_deviceContext, NULL ) != FALSE )
		{
			if ( wglDeleteContext( s_openGlRenderingContext ) == FALSE )
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to delete the OpenGL rendering context: " << GetLastWindowsError();
				DEBUG_PRINT( errorMessage.str() );
			}
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "Windows failed to unset the current OpenGL rendering context: " << GetLastWindowsError();
			DEBUG_PRINT( errorMessage.str() );
		}
		s_openGlRenderingContext = NULL;
	}

	if ( s_deviceContext != NULL )
	{
		// The documentation says that this call isn't necessary when CS_OWNDC is used
		ReleaseDC( s_renderingWindow, s_deviceContext );
		s_deviceContext = NULL;
	}

	s_renderingWindow = NULL;

	return !wereThereErrors;
}

// Helper Function Declarations
//=============================

namespace
{
	bool CreateProgram()
	{
		// Create a program
		{
			s_programId = glCreateProgram();
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to create a program: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				DEBUG_PRINT( errorMessage.str() );
				return false;
			}
			else if ( s_programId == 0 )
			{
				DEBUG_PRINT( "OpenGL failed to create a program" );
				return false;
			}
		}
		// Load and attach the shaders
		if ( !LoadVertexShader( s_programId ) )
		{
			return false;
		}
		if ( !LoadFragmentShader( s_programId ) )
		{
			return false;
		}
		// Link the program
		{
			glLinkProgram( s_programId );
			GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				// Get link info
				// (this won't be used unless linking fails
				// but it can be useful to look at when debugging)
				std::string linkInfo;
				{
					GLint infoSize;
					glGetProgramiv( s_programId, GL_INFO_LOG_LENGTH, &infoSize );
					errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						sLogInfo info( static_cast<size_t>( infoSize ) );
						GLsizei* dontReturnLength = NULL;
						glGetProgramInfoLog( s_programId, static_cast<GLsizei>( infoSize ), dontReturnLength, info.memory );
						errorCode = glGetError();
						if ( errorCode == GL_NO_ERROR )
						{
							linkInfo = info.memory;
						}
						else
						{
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get link info of the program: " <<
								reinterpret_cast<const char*>( gluErrorString( errorCode ) );
							DEBUG_PRINT( errorMessage.str() );
							return false;
						}
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the program link info: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						DEBUG_PRINT( errorMessage.str() );
						return false;
					}
				}
				// Check to see if there were link errors
				GLint didLinkingSucceed;
				{
					glGetProgramiv( s_programId, GL_LINK_STATUS, &didLinkingSucceed );
					errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						if ( didLinkingSucceed == GL_FALSE )
						{
							std::stringstream errorMessage;
							errorMessage << "The program failed to link:\n" << linkInfo;
							DEBUG_PRINT( errorMessage.str() );
							return false;
						}
					}
					else
					{
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if linking of the program succeeded: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						DEBUG_PRINT( errorMessage.str() );
						return false;
					}
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to link the program: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				DEBUG_PRINT( errorMessage.str() );
				return false;
			}
		}

		return true;
	}

	bool CreateRenderingContext()
	{
		// A "device context" can be thought of an abstraction that Windows uses
		// to represent the graphics adaptor used to display a given window
		s_deviceContext = GetDC( s_renderingWindow );
		if ( s_deviceContext == NULL )
		{
			DEBUG_PRINT( "Windows failed to get the device context" );
			return false;
		}
		// Windows requires that an OpenGL "render context" is made for the window we want to use to render into
		{
			// Set the pixel format of the rendering window
			{
				PIXELFORMATDESCRIPTOR desiredPixelFormat = { 0 };
				{
					desiredPixelFormat.nSize = sizeof( PIXELFORMATDESCRIPTOR );
					desiredPixelFormat.nVersion = 1;

					desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
					desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
					desiredPixelFormat.cColorBits = 32;
					desiredPixelFormat.iLayerType = PFD_MAIN_PLANE ;
				}
				// Get the ID of the desired pixel format
				int pixelFormatId;
				{
					pixelFormatId = ChoosePixelFormat( s_deviceContext, &desiredPixelFormat );
					if ( pixelFormatId == 0 )
					{
						std::stringstream errorMessage;
						errorMessage << "Windows couldn't choose the closest pixel format: " << eae6320::GetLastWindowsError();
						DEBUG_PRINT( errorMessage.str() );
						return false;
					}
				}
				// Set it
				if ( SetPixelFormat( s_deviceContext, pixelFormatId, &desiredPixelFormat ) == FALSE )
				{
					std::stringstream errorMessage;
					errorMessage << "Windows couldn't set the desired pixel format: " << eae6320::GetLastWindowsError();
					DEBUG_PRINT( errorMessage.str() );
					return false;
				}
			}
			// Create the OpenGL rendering context
			s_openGlRenderingContext = wglCreateContext( s_deviceContext );
			if ( s_openGlRenderingContext == NULL )
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to create an OpenGL rendering context: " << eae6320::GetLastWindowsError();
				DEBUG_PRINT( errorMessage.str() );
				return false;
			}
			// Set it as the rendering context of this thread
			if ( wglMakeCurrent( s_deviceContext, s_openGlRenderingContext ) == FALSE )
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to set the current OpenGL rendering context: " << eae6320::GetLastWindowsError();
				DEBUG_PRINT( errorMessage.str() );
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

	bool LoadFragmentShader( const GLuint i_programId )
	{
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv( GL_SHADER_COMPILER, &isShaderCompilingSupported );
			if ( !isShaderCompilingSupported )
			{
				DEBUG_PRINT( "Compiling shaders at run-time isn't supported on this implementation (this should never happen)" );
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
				const char* sourceCodeFileName = "data/fragment.shader";
				std::string errorMessage;
				if ( !LoadAndAllocateShaderProgram( sourceCodeFileName, shaderSource, fileSize, &errorMessage ) )
				{
					wereThereErrors = true;
					DEBUG_PRINT( errorMessage );
					goto OnExit;
				}
			}
			// Generate a shader
			fragmentShaderId = glCreateShader( GL_FRAGMENT_SHADER );
			{
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to get an unused fragment shader ID: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					DEBUG_PRINT( errorMessage.str() );
					goto OnExit;
				}
				else if ( fragmentShaderId == 0 )
				{
					wereThereErrors = true;
					DEBUG_PRINT( "OpenGL failed to get an unused fragment shader ID" );
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 3;
				const GLchar* shaderSources[] =
				{
					"#version 330\n", //version number must come first
					"#define EAE6320_PLATFORM_GL\n",
					reinterpret_cast<GLchar*>(shaderSource)
				};
				const GLint* sourcesAreNullTerminated = NULL;
				const GLint length = static_cast<GLuint>( fileSize );
				glShaderSource( fragmentShaderId, shaderSourceCount, shaderSources, sourcesAreNullTerminated);
				const GLenum errorCode = glGetError();
				if ( errorCode != GL_NO_ERROR )
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "OpenGL failed to set the fragment shader source code: " <<
						reinterpret_cast<const char*>( gluErrorString( errorCode ) );
					DEBUG_PRINT( errorMessage.str() );
					goto OnExit;
				}
			}
		}
		// Compile the shader source code
		{
			glCompileShader( fragmentShaderId );
			GLenum errorCode = glGetError();
			if ( errorCode == GL_NO_ERROR )
			{
				// Get compilation info
				// (this won't be used unless compilation fails
				// but it can be useful to look at when debugging)
				std::string compilationInfo;
				{
					GLint infoSize;
					glGetShaderiv( fragmentShaderId, GL_INFO_LOG_LENGTH, &infoSize );
					errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						sLogInfo info( static_cast<size_t>( infoSize ) );
						GLsizei* dontReturnLength = NULL;
						glGetShaderInfoLog( fragmentShaderId, static_cast<GLsizei>( infoSize ), dontReturnLength, info.memory );
						errorCode = glGetError();
						if ( errorCode == GL_NO_ERROR )
						{
							compilationInfo = info.memory;
						}
						else
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "OpenGL failed to get compilation info of the fragment shader source code: " <<
								reinterpret_cast<const char*>( gluErrorString( errorCode ) );
							DEBUG_PRINT( errorMessage.str() );
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the fragment shader compilation info: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						DEBUG_PRINT( errorMessage.str() );
						goto OnExit;
					}
				}
				// Check to see if there were compilation errors
				GLint didCompilationSucceed;
				{
					glGetShaderiv( fragmentShaderId, GL_COMPILE_STATUS, &didCompilationSucceed );
					errorCode = glGetError();
					if ( errorCode == GL_NO_ERROR )
					{
						if ( didCompilationSucceed == GL_FALSE )
						{
							wereThereErrors = true;
							std::stringstream errorMessage;
							errorMessage << "The fragment shader failed to compile:\n" << compilationInfo;
							DEBUG_PRINT( errorMessage.str() );
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the fragment shader source code succeeded: " <<
							reinterpret_cast<const char*>( gluErrorString( errorCode ) );
						DEBUG_PRINT( errorMessage.str() );
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to compile the fragment shader source code: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				DEBUG_PRINT( errorMessage.str() );
				goto OnExit;
			}
		}
		// Attach the shader to the program
		{
			glAttachShader( i_programId, fragmentShaderId );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to attach the fragment shader to the program: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				DEBUG_PRINT( errorMessage.str() );
				goto OnExit;
			}
		}

	OnExit:

		if ( fragmentShaderId != 0 )
		{
			// Even if the shader was successfully compiled
			// once it has been attached to the program we can (and should) delete our reference to it
			// (any associated memory that OpenGL has allocated internally will be freed
			// once the program is deleted)
			glDeleteShader( fragmentShaderId );
			const GLenum errorCode = glGetError();
			if ( errorCode != GL_NO_ERROR )
			{
				std::stringstream errorMessage;
				errorMessage << "OpenGL failed to delete the fragment shader ID: " <<
					reinterpret_cast<const char*>( gluErrorString( errorCode ) );
				DEBUG_PRINT( errorMessage.str() );
			}
			fragmentShaderId = 0;
		}
		if ( shaderSource != NULL )
		{
			free( shaderSource );
			shaderSource = NULL;
		}

		return !wereThereErrors;
	}

	bool LoadVertexShader(const GLuint i_programId)
	{
		// Verify that compiling shaders at run-time is supported
		{
			GLboolean isShaderCompilingSupported;
			glGetBooleanv(GL_SHADER_COMPILER, &isShaderCompilingSupported);
			if (!isShaderCompilingSupported)
			{
				DEBUG_PRINT("Compiling shaders at run-time isn't supported on this implementation (this should never happen)");
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
				const char* sourceCodeFileName = "data/vertex.shader";
				std::string errorMessage;
				if (!LoadAndAllocateShaderProgram(sourceCodeFileName, shaderSource, fileSize, &errorMessage))
				{
					wereThereErrors = true;
					DEBUG_PRINT(errorMessage);
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
					DEBUG_PRINT(errorMessage.str());
					goto OnExit;
				}
				else if (vertexShaderId == 0)
				{
					wereThereErrors = true;
					DEBUG_PRINT("OpenGL failed to get an unused vertex shader ID");
					goto OnExit;
				}
			}
			// Set the source code into the shader
			{
				const GLsizei shaderSourceCount = 3;
				const GLchar* shaderSources[] =
				{
					"#version 330\n", //version number must come first
					"#define EAE6320_PLATFORM_GL\n",
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
					DEBUG_PRINT(errorMessage.str());
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
							DEBUG_PRINT(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to get the length of the vertex shader compilation info: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						DEBUG_PRINT(errorMessage.str());
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
							DEBUG_PRINT(errorMessage.str());
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::stringstream errorMessage;
						errorMessage << "OpenGL failed to find out if compilation of the vertex shader source code succeeded: " <<
							reinterpret_cast<const char*>(gluErrorString(errorCode));
						DEBUG_PRINT(errorMessage.str());
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
				DEBUG_PRINT(errorMessage.str());
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
				DEBUG_PRINT(errorMessage.str());
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
				DEBUG_PRINT(errorMessage.str());
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
