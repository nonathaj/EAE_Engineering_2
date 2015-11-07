#ifndef _LAME_CONTEXT_H
#define _LAME_CONTEXT_H

#include <cstdint>
#include <string>

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Lame
{
	class Context
	{
	public:
		~Context();
		
		//Create a mesh with right-handed indices
		static Context* Create(const HWND i_renderingWindow);

		bool ClearScreen();

		bool BeginFrame();
		bool EndFrame();

		uint32_t screen_width();
		uint32_t screen_height();

#if EAE6320_PLATFORM_D3D
		IDirect3D9* get_direct3dInterface() const { return direct3dInterface; }
		IDirect3DDevice9* get_direct3dDevice() const { return direct3dDevice; }
#elif EAE6320_PLATFORM_GL
		HDC get_deviceContext() const { return deviceContext; }
		HGLRC get_openGlRenderingContext() const { return openGlRenderingContext; }
#endif

	private:

		Context(HWND i_renderingWindow) { renderingWindow = i_renderingWindow; }

		//Do not allow Contexts to be managed without pointers
		Context();
		Context(const Context &i_mesh);
		Context& operator=(const Context &i_mesh);

		HWND renderingWindow = nullptr;

#if EAE6320_PLATFORM_D3D
		IDirect3D9* direct3dInterface = nullptr;
		IDirect3DDevice9* direct3dDevice = nullptr;
#elif EAE6320_PLATFORM_GL
		HDC deviceContext = NULL;
		HGLRC openGlRenderingContext = NULL;
#endif
	};
}

#endif //_LAME_CONTEXT_H
