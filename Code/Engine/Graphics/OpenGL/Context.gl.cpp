
#include "../Context.h"

#include <cassert>
#include <sstream>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "../../System/UserOutput.h"
#include "../../Windows/Functions.h"
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"


namespace
{
	bool CreateRenderingContext(const HWND i_renderingWindow, HDC& o_deviceContext, HGLRC& o_openGlRenderingContext);
	void CleanupContextData(const HWND i_renderingWindow, HDC i_deviceContext, HGLRC i_openGlRenderingContext);
}

namespace Lame
{
	Context* Context::Create(const HWND i_renderingWindow)
	{
		HDC deviceContext = nullptr;
		HGLRC openGlRenderingContext = nullptr;

		if (CreateRenderingContext(i_renderingWindow, deviceContext, openGlRenderingContext))
		{
			std::string error;
			if (eae6320::OpenGlExtensions::Load(&error))
			{
				Context *context = new Context(i_renderingWindow);
				if (context)
				{
					context->deviceContext = deviceContext;
					context->openGlRenderingContext = openGlRenderingContext;
					return context;
				}
				else
				{
					System::UserOutput::Display("Failed to create OpenGL Context, due to insufficient memory.", "Context Loading Error");
				}
			}
			else
			{
				System::UserOutput::Display(error);
			}
		}
		
		CleanupContextData(i_renderingWindow, deviceContext, openGlRenderingContext);
		return nullptr;
	}

	Context::~Context()
	{
		CleanupContextData(renderingWindow, deviceContext, openGlRenderingContext);
	}

	bool Context::ClearScreen()
	{
		// Black is usually used
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		bool success = glGetError() == GL_NO_ERROR;
		// In addition to the color, "depth" and "stencil" can also be cleared,
		// but for now we only care about color
		const GLbitfield clearColor = GL_COLOR_BUFFER_BIT;
		glClear(clearColor);
		success = success && glGetError() == GL_NO_ERROR;

		return success;
	}

	bool Context::BeginFrame()
	{
		return true;
	}

	bool Context::EndFrame()
	{
		// Everything has been drawn to the "back buffer", which is just an image in memory.
		// In order to display it, the contents of the back buffer must be swapped with the "front buffer"
		// (which is what the user sees)
		{
			BOOL result = SwapBuffers(deviceContext);
			assert(result != FALSE);
		}
		return true;
	}
}

namespace
{
	bool CreateRenderingContext(const HWND i_renderingWindow, HDC& o_deviceContext, HGLRC& o_openGlRenderingContext)
	{
		// A "device context" can be thought of an abstraction that Windows uses
		// to represent the graphics adaptor used to display a given window
		o_deviceContext = GetDC(i_renderingWindow);
		if (o_deviceContext == NULL)
		{
			System::UserOutput::Display("Windows failed to get the device context");
			return false;
		}
		// Windows requires that an OpenGL "render context" is made for the window we want to use to render into
		{
			// Set the pixel format of the rendering window
			{
				PIXELFORMATDESCRIPTOR desiredPixelFormat = { 0 };
				{
					desiredPixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
					desiredPixelFormat.nVersion = 1;

					desiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
					desiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
					desiredPixelFormat.cColorBits = 32;
					desiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
				}
				// Get the ID of the desired pixel format
				int pixelFormatId;
				{
					pixelFormatId = ChoosePixelFormat(o_deviceContext, &desiredPixelFormat);
					if (pixelFormatId == 0)
					{
						std::stringstream errorMessage;
						errorMessage << "Windows couldn't choose the closest pixel format: " << eae6320::GetLastWindowsError();
						System::UserOutput::Display(errorMessage.str());
						return false;
					}
				}
				// Set it
				if (SetPixelFormat(o_deviceContext, pixelFormatId, &desiredPixelFormat) == FALSE)
				{
					std::stringstream errorMessage;
					errorMessage << "Windows couldn't set the desired pixel format: " << eae6320::GetLastWindowsError();
					System::UserOutput::Display(errorMessage.str());
					return false;
				}
			}
			// Create the OpenGL rendering context
			o_openGlRenderingContext = wglCreateContext(o_deviceContext);
			if (o_openGlRenderingContext == NULL)
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to create an OpenGL rendering context: " << eae6320::GetLastWindowsError();
				System::UserOutput::Display(errorMessage.str());
				return false;
			}
			// Set it as the rendering context of this thread
			if (wglMakeCurrent(o_deviceContext, o_openGlRenderingContext) == FALSE)
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to set the current OpenGL rendering context: " << eae6320::GetLastWindowsError();
				System::UserOutput::Display(errorMessage.str());
				return false;
			}
		}

		//enable face culling
		glEnable(GL_CULL_FACE);
		const GLenum errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			std::stringstream errorMessage;
			errorMessage << "OpenGL failed to delete the vertex array: " << reinterpret_cast<const char*>(gluErrorString(errorCode));
			System::UserOutput::Display(errorMessage.str());
		}

		return true;
	}

	void CleanupContextData(const HWND i_renderingWindow, HDC i_deviceContext, HGLRC i_openGlRenderingContext)
	{
		if (i_openGlRenderingContext != nullptr)
		{
			if (wglMakeCurrent(i_deviceContext, nullptr) != FALSE)
			{
				if (wglDeleteContext(i_openGlRenderingContext) == FALSE)
				{
					std::stringstream errorMessage;
					errorMessage << "Windows failed to delete the OpenGL rendering context: " << eae6320::GetLastWindowsError();
					System::UserOutput::Display(errorMessage.str());
				}
			}
			else
			{
				std::stringstream errorMessage;
				errorMessage << "Windows failed to unset the current OpenGL rendering context: " << eae6320::GetLastWindowsError();
				System::UserOutput::Display(errorMessage.str());
			}
		}
		if (i_deviceContext != nullptr)
		{
			// The documentation says that this call isn't necessary when CS_OWNDC is used
			ReleaseDC(i_renderingWindow, i_deviceContext);
		}
	}
}
