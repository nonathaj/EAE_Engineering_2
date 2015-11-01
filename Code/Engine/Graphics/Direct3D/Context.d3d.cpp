
#include "../Context.h"

#include <cassert>
#include <d3dx9shader.h>

#include "../../System/UserOutput.h"

namespace
{
	bool CreateDevice(IDirect3D9* _direct3dInterface, const HWND i_renderingWindow, IDirect3DDevice9*& direct3dDevice);
	bool CreateInterface(IDirect3D9*& o_direct3dInterface);
}

namespace Lame
{
	Context* Context::Create(const HWND i_renderingWindow)
	{
		IDirect3D9* direct3dInterface = nullptr;
		IDirect3DDevice9* direct3dDevice = nullptr;

		if (!CreateInterface(direct3dInterface) || !CreateDevice(direct3dInterface, i_renderingWindow, direct3dDevice))
			goto OnError;

		Context *context = new Context(i_renderingWindow);
		if (context)
		{
			context->direct3dDevice = direct3dDevice;
			context->direct3dInterface = direct3dInterface;
		}
		else
		{
			System::UserOutput::Display("Failed to create Direct3D Context, due to insufficient memory.", "Context Loading Error");
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

	bool Context::ClearScreen()
	{
		const D3DRECT* subRectanglesToClear = NULL;
		const DWORD subRectangleCount = 0;
		const DWORD clearTheRenderTarget = D3DCLEAR_TARGET;
		D3DCOLOR clearColor;
		{
			// Black is usually used:
			clearColor = D3DCOLOR_XRGB(0, 0, 0);
		}
		const float noZBuffer = 0.0f;
		const DWORD noStencilBuffer = 0;
		HRESULT result = direct3dDevice->Clear(subRectangleCount, subRectanglesToClear, clearTheRenderTarget, clearColor, noZBuffer, noStencilBuffer);
		return SUCCEEDED(result);
	}

	bool Context::BeginFrame()
	{
		HRESULT result = direct3dDevice->BeginScene();
		return SUCCEEDED(result);
	}

	bool Context::EndFrame()
	{
		HRESULT result = direct3dDevice->EndScene();
		bool success = SUCCEEDED(result);

		// Everything has been drawn to the "back buffer", which is just an image in memory.
		// In order to display it, the contents of the back buffer must be "presented"
		// (to the front buffer)
		{
			const RECT* noSourceRectangle = NULL;
			const RECT* noDestinationRectangle = NULL;
			const HWND useDefaultWindow = NULL;
			const RGNDATA* noDirtyRegion = NULL;
			HRESULT result = direct3dDevice->Present(noSourceRectangle, noDestinationRectangle, useDefaultWindow, noDirtyRegion);
			success = success && SUCCEEDED(result);
		}
		return success;
	}
}

namespace
{
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
			presentationParameters.EnableAutoDepthStencil = FALSE;
			presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		}
		HRESULT result = i_direct3dInterface->CreateDevice(useDefaultDevice, useHardwareRendering,
			i_renderingWindow, useHardwareVertexProcessing, &presentationParameters, &o_direct3dDevice);
		if (SUCCEEDED(result))
		{
			return true;
		}
		else
		{
			System::UserOutput::Display("Direct3D failed to create a Direct3D9 device");
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
			System::UserOutput::Display("DirectX failed to create a Direct3D9 interface");
			return false;
		}
	}
}
