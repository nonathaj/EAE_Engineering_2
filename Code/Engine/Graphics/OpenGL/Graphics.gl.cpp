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
#include "../Effect.h"
#include "../Vertex.h"

// Static Data Initialization
//===========================

namespace
{
	HWND s_renderingWindow = NULL;
	HDC s_deviceContext = NULL;
	HGLRC s_openGlRenderingContext = NULL;

	Lame::Effect *effect = nullptr;
	Lame::Mesh *squareMesh = nullptr;
	Lame::Mesh *triangleMesh = nullptr;
}

// Helper Function Declarations
//=============================

namespace
{
	bool CreateRenderingContext();
	bool LoadAndAllocateShaderProgram( const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage );

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
	if ( !(squareMesh = Lame::Mesh::Create("data/square.mesh")) )
	{
		MessageBox(i_renderingWindow, "Failed to load the square.mesh file.", "Mesh loading error", 0);
		goto OnError;
	}
	if ( !(triangleMesh = Lame::Mesh::Create("data/triangle.mesh")) )
	{
		MessageBox(i_renderingWindow, "Failed to load the triangle.mesh file.", "Mesh loading error", 0);
		goto OnError;
	}

	if ( !(effect = Lame::Effect::Create("data/vertex.shader", "data/fragment.shader")) )
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

	//bind the effect
	{
		bool bindSuccess = effect->Bind();
		assert(bindSuccess);
	}

	// The actual function calls that draw geometry
	{
		bool drawSuccess = squareMesh->Draw();
		assert(drawSuccess);

		drawSuccess = triangleMesh->Draw();
		assert(drawSuccess);
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
		if (effect)
		{
			delete effect;
			effect = nullptr;
		}

		if (squareMesh)
		{
			delete squareMesh;
			squareMesh = nullptr;
		}
		if (triangleMesh)
		{
			delete triangleMesh;
			triangleMesh = nullptr;
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
}
