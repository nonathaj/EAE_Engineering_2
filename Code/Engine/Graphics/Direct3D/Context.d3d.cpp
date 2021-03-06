
#include "../Context.h"

#include <cassert>
#include <d3dx9shader.h>
#include <d3d9types.h>

#include "../../System/UserOutput.h"
#include "../../Core/Rectangle2D.h"

namespace
{
	bool CreateDevice(IDirect3D9* _direct3dInterface, const HWND i_renderingWindow, IDirect3DDevice9*& direct3dDevice);
	bool CreateInterface(IDirect3D9*& o_direct3dInterface);
	bool SetSamplerState(IDirect3DDevice9* direct3dDevice);
}

namespace Lame
{
	Context* Context::Create(const HWND i_renderingWindow)
	{
		IDirect3D9* direct3dInterface = nullptr;
		IDirect3DDevice9* direct3dDevice = nullptr;

		if (!CreateInterface(direct3dInterface) || 
			!CreateDevice(direct3dInterface, i_renderingWindow, direct3dDevice) ||
			!SetSamplerState(direct3dDevice) )
			goto OnError;

		Context *context = new Context(i_renderingWindow);
		if (context)
		{
			context->direct3dDevice = direct3dDevice;
			context->direct3dInterface = direct3dInterface;
		}
		else
		{
			Lame::UserOutput::Display("Failed to create Direct3D Context, due to insufficient memory.", "Context Loading Error");
		}
		return context;

	OnError:
		if (direct3dInterface)
		{
			if (direct3dDevice)
			{
				direct3dDevice->Release();
				direct3dDevice = nullptr;
			}
			direct3dInterface->Release();
			direct3dInterface = nullptr;
		}
		return nullptr;
	}

	Context::~Context()
	{
		if (direct3dInterface)
		{
			if (direct3dDevice)
			{
				direct3dDevice->Release();
				direct3dDevice = nullptr;
			}
			direct3dInterface->Release();
			direct3dInterface = nullptr;
		}
		renderingWindow = nullptr;
	}

	bool Context::Clear(bool screen, bool depth, bool stencil)
	{
		const D3DRECT* subRectanglesToClear = nullptr;
		const DWORD subRectangleCount = 0;
		const DWORD buffersToClear = (screen ? D3DCLEAR_TARGET : 0x0) | (depth ? D3DCLEAR_ZBUFFER : 0x0) | (stencil ? D3DCLEAR_STENCIL : 0x0);
		const D3DCOLOR clearColor = D3DCOLOR_COLORVALUE(screen_clear_color.r(), screen_clear_color.g(), screen_clear_color.b(), screen_clear_color.a());
		const float depthBuffer = 1.0f;
		const DWORD stencilBuffer = 0;
		HRESULT result = direct3dDevice->Clear(subRectangleCount, subRectanglesToClear, buffersToClear, clearColor, depthBuffer, stencilBuffer);
		return SUCCEEDED(result);
	}

	void Context::set_screen_clear_color(const Color& i_screen_clear_color)
	{
		screen_clear_color = i_screen_clear_color;
	}

	bool Context::BeginFrame()
	{
		return SUCCEEDED(direct3dDevice->BeginScene());
	}

	bool Context::EndFrame()
	{
		bool success = SUCCEEDED(direct3dDevice->EndScene());

		// Everything has been drawn to the "back buffer", which is just an image in memory.
		// In order to display it, the contents of the back buffer must be "presented"
		// (to the front buffer)
		{
			HRESULT result = direct3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
			success = success && SUCCEEDED(result);
		}
		return success;
	}

	HRESULT Context::GetVertexProcessingUsage(DWORD& o_usage)
	{
		D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
		const HRESULT result = get_direct3dDevice()->GetCreationParameters(&deviceCreationParameters);
		if (SUCCEEDED(result))
		{
			DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
				(D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
			o_usage = (vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
		}
		else
		{
			Lame::UserOutput::Display("Direct3D failed to get the device's creation parameters");
		}
		return result;
	}

	bool Context::SetVertexFormat(IDirect3DVertexDeclaration9 ** o_vertex_declaration)
	{
		// These elements must match the Vertex layout exactly.
		// They instruct Direct3D how to match the binary data in the vertex buffer
		// to the input elements in a vertex shader
		// (by using D3DDECLUSAGE enums here and semantics in the shader,
		// so that, for example, D3DDECLUSAGE_POSITION here matches with POSITION in shader code).
		// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
		D3DVERTEXELEMENT9 vertexElements[] =
		{
			// Stream 0

			// POSITION, 3 floats == 12 bytes, Offset = 0
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

			// TEXCOORD0, 2 floats == 8 bytes, Offset = 12
			{ 0, 12 , D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

			// COLOR0,  D3DCOLOR == 4 bytes, Offset = 20
			{ 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

			// The following marker signals the end of the vertex declaration
			D3DDECL_END()
		};
		HRESULT result = get_direct3dDevice()->CreateVertexDeclaration(vertexElements, o_vertex_declaration);
		if (SUCCEEDED(result))
		{
			result = get_direct3dDevice()->SetVertexDeclaration(*o_vertex_declaration);
			if (FAILED(result))
			{
				Lame::UserOutput::Display("Direct3D failed to set the vertex declaration");
				return false;
			}
		}
		else
		{
			Lame::UserOutput::Display("Direct3D failed to create a Direct3D9 vertex declaration");
			return false;
		}
		return true;
	}

	Rectangle2D Context::GetRealScreenCoord(const Rectangle2D& i_virtual_screen_coord)
	{
		return Lame::Rectangle2D(
			i_virtual_screen_coord.left() * 2.0f - 1.0f,
			i_virtual_screen_coord.right() * 2.0f - 1.0f,
			i_virtual_screen_coord.top() * 2.0f - 1.0f,
			i_virtual_screen_coord.bottom() * 2.0f - 1.0f);
	}

	Rectangle2D Context::GetVirtualScreenCoord(const Rectangle2D& i_real_screen_coord)
	{
		return Lame::Rectangle2D(
			(i_real_screen_coord.left() + 1.0f) / 2.0f,
			(i_real_screen_coord.right() + 1.0f) / 2.0f,
			(i_real_screen_coord.top() + 1.0f) / 2.0f,
			(i_real_screen_coord.bottom() + 1.0f) / 2.0f);
	}
}

namespace
{
	bool SetSamplerState(IDirect3DDevice9* direct3dDevice)
	{
		for (DWORD i = 0; i < 8; ++i)
		{
			if (FAILED(direct3dDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)))
				return false;
			if (FAILED(direct3dDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)))
				return false;
			if (FAILED(direct3dDevice->SetSamplerState(i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR)))
				return false;
		}
		return true;
	}

	bool CreateDevice(IDirect3D9* i_direct3dInterface, const HWND i_renderingWindow, IDirect3DDevice9*& o_direct3dDevice)
	{
		const UINT useDefaultDevice = D3DADAPTER_DEFAULT;
		const D3DDEVTYPE useHardwareRendering = D3DDEVTYPE_HAL;
		const DWORD useHardwareVertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		D3DPRESENT_PARAMETERS presentationParameters = { 0 };
		{
			{
				const unsigned int useRenderingWindowDimensions = 0;
				presentationParameters.BackBufferWidth = useRenderingWindowDimensions;
				presentationParameters.BackBufferHeight = useRenderingWindowDimensions;
			}
			presentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
			presentationParameters.BackBufferCount = 1;
			presentationParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
			presentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
			presentationParameters.hDeviceWindow = i_renderingWindow;
			presentationParameters.Windowed = TRUE;
			presentationParameters.EnableAutoDepthStencil = TRUE;
			presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
			presentationParameters.AutoDepthStencilFormat = D3DFORMAT::D3DFMT_D24S8;
		}
		HRESULT result = i_direct3dInterface->CreateDevice(useDefaultDevice, useHardwareRendering,
			i_renderingWindow, useHardwareVertexProcessing, &presentationParameters, &o_direct3dDevice);
		if (SUCCEEDED(result))
		{
			return true;
		}
		else
		{
			Lame::UserOutput::Display("Direct3D failed to create a Direct3D9 device");
			return false;
		}
	}

	bool CreateInterface(IDirect3D9*& o_direct3dInterface)
	{
		// D3D_SDK_VERSION is #defined by the Direct3D header files,
		// and is just a way to make sure that everything is up-to-date
		o_direct3dInterface = Direct3DCreate9(D3D_SDK_VERSION);
		if (o_direct3dInterface)
		{
			return true;
		}
		else
		{
			Lame::UserOutput::Display("DirectX failed to create a Direct3D9 interface");
			return false;
		}
	}
}
