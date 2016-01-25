#ifndef _LAME_CONTEXT_H
#define _LAME_CONTEXT_H

#include <cstdint>
#include <string>

#include "Color.h"

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

		bool Clear(bool screen, bool depth, bool stencil);

		bool BeginFrame();
		bool EndFrame();

		uint32_t screen_width() const;
		uint32_t screen_height() const;

		Color get_screen_clear_color() const { return screen_clear_color; }
		void set_screen_clear_color(const Color& i_screen_clear_color);

#if EAE6320_PLATFORM_D3D
		IDirect3D9* get_direct3dInterface() const { return direct3dInterface; }
		IDirect3DDevice9* get_direct3dDevice() const { return direct3dDevice; }
		HRESULT GetVertexProcessingUsage(DWORD& o_usage);
#elif EAE6320_PLATFORM_GL
		HDC get_deviceContext() const { return deviceContext; }
		HGLRC get_openGlRenderingContext() const { return openGlRenderingContext; }
#endif

	private:
		Context(HWND i_renderingWindow);

		//Do not allow Contexts to be managed without pointers
		Context();
		Context(const Context &i_other);
		Context& operator=(const Context &i_other);

		Color screen_clear_color;
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
