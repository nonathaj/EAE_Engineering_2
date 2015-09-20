// Header Files
//=============

#include "../Graphics.h"

#include <cassert>
#include <cstdint>
#include <d3d9.h>
#include <d3dx9shader.h>
#include <sstream>
#include "../../System/Console.h"

#include "../Vertex.h"
#include "../Mesh.h"

// Static Data Initialization
//===========================

namespace
{
	HWND s_renderingWindow = NULL;
	IDirect3D9* s_direct3dInterface = NULL;
	IDirect3DDevice9* s_direct3dDevice = NULL;

	// The vertex shader is a program that operates on vertices.
	// Its input comes from a C/C++ "draw call" and is:
	//	* Position
	//	* Any other data we want
	// Its output is:
	//	* Position
	//		(So that the graphics hardware knows which pixels to fill in for the triangle)
	//	* Any other data we want
	IDirect3DVertexShader9* s_vertexShader = NULL;
	// The fragment shader is a program that operates on fragments
	// (or potential pixels).
	// Its input is:
	//	* The data that was output from the vertex shader,
	//		interpolated based on how close the fragment is
	//		to each vertex in the triangle.
	// Its output is:
	//	* The final color that the pixel should be
	IDirect3DPixelShader9* s_fragmentShader = NULL;

	Lame::Mesh *mesh = nullptr;
}

// Helper Function Declarations
//=============================

namespace
{
	bool CreateDevice();
	bool CreateInterface();
	HRESULT GetVertexProcessingUsage( DWORD& o_usage );
	bool LoadFragmentShader();
	bool LoadVertexShader();

	bool CreateMesh();
}

// Interface
//==========


IDirect3DDevice9* eae6320::Graphics::get_direct3dDevice()
{
	return s_direct3dDevice;
}

bool eae6320::Graphics::Initialize( const HWND i_renderingWindow )
{
	s_renderingWindow = i_renderingWindow;

	// Initialize the interface to the Direct3D9 library
	if ( !CreateInterface() )
	{
		return false;
	}
	// Create an interface to a Direct3D device
	if ( !CreateDevice() )
	{
		goto OnError;
	}

	// Initialize the graphics objects
	if (!CreateMesh())
		goto OnError;

	if ( !LoadVertexShader() )
	{
		goto OnError;
	}
	if ( !LoadFragmentShader() )
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
		const D3DRECT* subRectanglesToClear = NULL;
		const DWORD subRectangleCount = 0;
		const DWORD clearTheRenderTarget = D3DCLEAR_TARGET;
		D3DCOLOR clearColor;
		{
			// Black is usually used:
			clearColor = D3DCOLOR_XRGB( 0, 0, 0 );
		}
		const float noZBuffer = 0.0f;
		const DWORD noStencilBuffer = 0;
		HRESULT result = s_direct3dDevice->Clear( subRectangleCount, subRectanglesToClear,
			clearTheRenderTarget, clearColor, noZBuffer, noStencilBuffer );
		assert( SUCCEEDED( result ) );
	}

	// The actual function calls that draw geometry must be made between paired calls to
	// BeginScene() and EndScene()
	{
		HRESULT result = s_direct3dDevice->BeginScene();
		assert( SUCCEEDED( result ) );
		{
			// Set the shaders
			{
				result = s_direct3dDevice->SetVertexShader( s_vertexShader );
				assert( SUCCEEDED( result ) );
				result = s_direct3dDevice->SetPixelShader( s_fragmentShader );
				assert( SUCCEEDED( result ) );
			}
			result = mesh->Draw();
			assert(SUCCEEDED(result));
		}
		result = s_direct3dDevice->EndScene();
		assert( SUCCEEDED( result ) );
	}

	// Everything has been drawn to the "back buffer", which is just an image in memory.
	// In order to display it, the contents of the back buffer must be "presented"
	// (to the front buffer)
	{
		const RECT* noSourceRectangle = NULL;
		const RECT* noDestinationRectangle = NULL;
		const HWND useDefaultWindow = NULL;
		const RGNDATA* noDirtyRegion = NULL;
		HRESULT result = s_direct3dDevice->Present( noSourceRectangle, noDestinationRectangle, useDefaultWindow, noDirtyRegion );
		assert( SUCCEEDED( result ) );
	}
}

bool eae6320::Graphics::ShutDown()
{
	bool wereThereErrors = false;

	if ( s_direct3dInterface )
	{
		if ( s_direct3dDevice )
		{
			if ( s_vertexShader )
			{
				s_vertexShader->Release();
				s_vertexShader = NULL;
			}
			if ( s_fragmentShader )
			{
				s_fragmentShader->Release();
				s_fragmentShader = NULL;
			}

			delete mesh;
			mesh = nullptr;

			s_direct3dDevice->Release();
			s_direct3dDevice = NULL;
		}

		s_direct3dInterface->Release();
		s_direct3dInterface = NULL;
	}
	s_renderingWindow = NULL;

	return !wereThereErrors;
}

// Helper Function Definitions
//============================

namespace
{
	bool CreateDevice()
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
			presentationParameters.hDeviceWindow = s_renderingWindow;
			presentationParameters.Windowed = TRUE;
			presentationParameters.EnableAutoDepthStencil = FALSE;
			presentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
		}
		HRESULT result = s_direct3dInterface->CreateDevice( useDefaultDevice, useHardwareRendering,
			s_renderingWindow, useHardwareVertexProcessing, &presentationParameters, &s_direct3dDevice );
		if ( SUCCEEDED( result ) )
		{
			return true;
		}
		else
		{
			DEBUG_PRINT( "Direct3D failed to create a Direct3D9 device" );
			return false;
		}
	}

	bool CreateInterface()
	{
		// D3D_SDK_VERSION is #defined by the Direct3D header files,
		// and is just a way to make sure that everything is up-to-date
		s_direct3dInterface = Direct3DCreate9( D3D_SDK_VERSION );
		if ( s_direct3dInterface )
		{
			return true;
		}
		else
		{
			DEBUG_PRINT( "DirectX failed to create a Direct3D9 interface" );
			return false;
		}
	}

	HRESULT GetVertexProcessingUsage( DWORD& o_usage )
	{
		D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
		const HRESULT result = s_direct3dDevice->GetCreationParameters( &deviceCreationParameters );
		if ( SUCCEEDED( result ) )
		{
			DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
				( D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING );
			o_usage = ( vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING ) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
		}
		else
		{
			DEBUG_PRINT( "Direct3D failed to get the device's creation parameters" );
		}
		return result;
	}

	bool LoadFragmentShader()
	{
		// Load the source code from file and compile it
		ID3DXBuffer* compiledShader;
		{
			const char* sourceCodeFileName = "data/fragment.shader";
			const D3DXMACRO defines[] =
			{
				{ "EAE6320_PLATFORM_D3D", "1" },
				{ NULL, NULL }
			};
			ID3DXInclude* noIncludes = NULL;
			const char* entryPoint = "main";
			const char* profile = "ps_3_0";
			const DWORD noFlags = 0;
			ID3DXBuffer* errorMessages = NULL;
			ID3DXConstantTable** noConstants = NULL;
			HRESULT result = D3DXCompileShaderFromFile( sourceCodeFileName, defines, noIncludes, entryPoint, profile, noFlags,
				&compiledShader, &errorMessages, noConstants );
			if ( SUCCEEDED( result ) )
			{
				if ( errorMessages )
				{
					errorMessages->Release();
				}
			}
			else
			{
				if ( errorMessages )
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the fragment shader from the file " << sourceCodeFileName
						<< ":\n" << reinterpret_cast<char*>( errorMessages->GetBufferPointer() );
					DEBUG_PRINT( errorMessage.str() );
					errorMessages->Release();
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the fragment shader from the file " << sourceCodeFileName;
					DEBUG_PRINT( errorMessage.str() );
				}
				return false;
			}
		}
		// Create the fragment shader object
		bool wereThereErrors = false;
		{
			HRESULT result = s_direct3dDevice->CreatePixelShader( reinterpret_cast<DWORD*>( compiledShader->GetBufferPointer() ),
				&s_fragmentShader );
			if ( FAILED( result ) )
			{
				DEBUG_PRINT( "Direct3D failed to create the fragment shader" );
				wereThereErrors = true;
			}
			compiledShader->Release();
		}
		return !wereThereErrors;
	}

	bool LoadVertexShader()
	{
		// Load the source code from file and compile it
		ID3DXBuffer* compiledShader;
		{
			const char* sourceCodeFileName = "data/vertex.shader";
			const D3DXMACRO defines[] =
			{
				{ "EAE6320_PLATFORM_D3D", "1" },
				{ NULL, NULL }
			};
			ID3DXInclude* noIncludes = NULL;
			const char* entryPoint = "main";
			const char* profile = "vs_3_0";
			const DWORD noFlags = 0;
			ID3DXBuffer* errorMessages = NULL;
			ID3DXConstantTable** noConstants = NULL;
			HRESULT result = D3DXCompileShaderFromFile( sourceCodeFileName, defines, noIncludes, entryPoint, profile, noFlags,
				&compiledShader, &errorMessages, noConstants );
			if ( SUCCEEDED( result ) )
			{
				if ( errorMessages )
				{
					errorMessages->Release();
				}
			}
			else
			{
				if ( errorMessages )
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the vertex shader from the file " << sourceCodeFileName
						<< ":\n" << reinterpret_cast<char*>( errorMessages->GetBufferPointer() );
					DEBUG_PRINT( errorMessage.str() );
					errorMessages->Release();
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the vertex shader from the file " << sourceCodeFileName;
					DEBUG_PRINT( errorMessage.str() );
				}
				return false;
			}
		}
		// Create the vertex shader object
		bool wereThereErrors = false;
		{
			HRESULT result = s_direct3dDevice->CreateVertexShader( reinterpret_cast<DWORD*>( compiledShader->GetBufferPointer() ),
				&s_vertexShader );
			if ( FAILED( result ) )
			{
				DEBUG_PRINT( "Direct3D failed to create the vertex shader" );
				wereThereErrors = true;
			}
			compiledShader->Release();
		}
		return !wereThereErrors;
	}

	bool CreateMesh()
	{
		const size_t vertexCount = 4;
		Lame::Vertex vertexData[vertexCount];
		// Fill in the data for the triangle
		{
			// You will need to fill in two pieces of information for each vertex:
			//	* 2 floats for the POSITION
			//	* 4 uint8_ts for the COLOR

			// The floats for POSITION are for the X and Y coordinates, like in Assignment 02.
			// The difference this time is that there should be fewer (because we are sharing data).

			// The uint8_ts for COLOR are "RGBA", where "RGB" stands for "Red Green Blue" and "A" for "Alpha".
			// Conceptually each of these values is a [0,1] value, but we store them as an 8-bit value to save space
			// (color doesn't need as much precision as position),
			// which means that the data we send to the GPU will be [0,255].
			// For now the alpha value should _always_ be 255, and so you will choose color by changing the first three RGB values.
			// To make white you should use (255, 255, 255), to make black (0, 0, 0).
			// To make pure red you would use the max for R and nothing for G and B, so (1, 0, 0).
			// Experiment with other values to see what happens!

			vertexData[0].x = 0.0f;
			vertexData[0].y = 0.0f;
			vertexData[0].r = 0;
			vertexData[0].g = 255;
			vertexData[0].b = 0;
			vertexData[0].a = 255;

			vertexData[1].x = 0.0f;
			vertexData[1].y = 1.0f;
			vertexData[1].r = 0;
			vertexData[1].g = 0;
			vertexData[1].b = 255;
			vertexData[1].a = 255;

			vertexData[2].x = 1.0f;
			vertexData[2].y = 0.0f;
			vertexData[2].r = 255;
			vertexData[2].g = 0;
			vertexData[2].b = 0;
			vertexData[2].a = 255;

			vertexData[3].x = 1.0f;
			vertexData[3].y = 1.0f;
			vertexData[3].r = 255;
			vertexData[3].g = 255;
			vertexData[3].b = 0;
			vertexData[3].a = 255;
		}

		const size_t indexCount = 6;
		uint32_t indexData[indexCount];
		// Fill in the data for the triangle
		{
			// Triangle 0
			indexData[0] = 0;
			indexData[1] = 3;
			indexData[2] = 2;

			// Triangle 1
			indexData[3] = 0;
			indexData[4] = 1;
			indexData[5] = 3;
		}

		mesh = Lame::Mesh::CreateLeftHanded(vertexData, vertexCount, indexData, indexCount);
		return mesh != nullptr;
	}
}
