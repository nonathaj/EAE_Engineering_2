
#include "../Effect.h"

#include "../../Windows/Includes.h"
#include "../../Windows/Functions.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sstream>
#include <d3dx9shader.h>

#include "../Graphics.h"
#include "../../System/Console.h"

namespace
{
	bool LoadFragmentShader(std::string i_path, IDirect3DPixelShader9*& o_fragmentShader);
	bool LoadVertexShader(std::string i_path, IDirect3DVertexShader9*& o_vertexShader);
}

namespace Lame
{
	Effect* Effect::Create(std::string i_vertex_path, std::string i_fragment_path)
	{
		// The vertex shader is a program that operates on vertices.
		// Its input comes from a C/C++ "draw call" and is:
		//	* Position
		//	* Any other data we want
		// Its output is:
		//	* Position
		//		(So that the graphics hardware knows which pixels to fill in for the triangle)
		//	* Any other data we want
		IDirect3DVertexShader9* vertexShader = nullptr;
		// The fragment shader is a program that operates on fragments
		// (or potential pixels).
		// Its input is:
		//	* The data that was output from the vertex shader,
		//		interpolated based on how close the fragment is
		//		to each vertex in the triangle.
		// Its output is:
		//	* The final color that the pixel should be
		IDirect3DPixelShader9* fragmentShader = nullptr;

		if (!LoadFragmentShader(i_fragment_path, fragmentShader) || !LoadVertexShader(i_vertex_path, vertexShader))
			return nullptr;

		Effect *effect = new Effect();
		if (effect)
		{
			effect->vertexShader = vertexShader;
			effect->pixelShader = fragmentShader;
		}
		return effect;
	}

	bool Effect::Bind()
	{
		HRESULT result = eae6320::Graphics::get_direct3dDevice()->SetVertexShader(vertexShader);
		bool success = SUCCEEDED(result);
		assert(success);
		result = eae6320::Graphics::get_direct3dDevice()->SetPixelShader(pixelShader);
		success = success && SUCCEEDED(result);
		assert(success);
		return success;
	}

	Effect::~Effect()
	{
		if (vertexShader)
		{
			vertexShader->Release();
			vertexShader = nullptr;
		}
		if (pixelShader)
		{
			pixelShader->Release();
			pixelShader = nullptr;
		}
	}
}

namespace
{
	bool LoadFragmentShader(std::string i_path, IDirect3DPixelShader9*& o_fragmentShader)
	{
		// Load the source code from file and compile it
		ID3DXBuffer* compiledShader;
		{
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
			HRESULT result = D3DXCompileShaderFromFile(i_path.c_str(), defines, noIncludes, entryPoint, profile, noFlags,
				&compiledShader, &errorMessages, noConstants);
			if (SUCCEEDED(result))
			{
				if (errorMessages)
				{
					errorMessages->Release();
				}
			}
			else
			{
				if (errorMessages)
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the fragment shader from the file " << i_path
						<< ":\n" << reinterpret_cast<char*>(errorMessages->GetBufferPointer());
					DEBUG_PRINT(errorMessage.str());
					errorMessages->Release();
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the fragment shader from the file " << i_path;
					DEBUG_PRINT(errorMessage.str());
				}
				return false;
			}
		}
		// Create the fragment shader object
		bool wereThereErrors = false;
		{
			HRESULT result = eae6320::Graphics::get_direct3dDevice()->CreatePixelShader(reinterpret_cast<DWORD*>(compiledShader->GetBufferPointer()),
				&o_fragmentShader);
			if (FAILED(result))
			{
				DEBUG_PRINT("Direct3D failed to create the fragment shader");
				wereThereErrors = true;
			}
			compiledShader->Release();
		}
		return !wereThereErrors;
	}

	bool LoadVertexShader(std::string i_path, IDirect3DVertexShader9*& o_vertexShader)
	{
		// Load the source code from file and compile it
		ID3DXBuffer* compiledShader;
		{
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
			HRESULT result = D3DXCompileShaderFromFile(i_path.c_str(), defines, noIncludes, entryPoint, profile, noFlags,
				&compiledShader, &errorMessages, noConstants);
			if (SUCCEEDED(result))
			{
				if (errorMessages)
				{
					errorMessages->Release();
				}
			}
			else
			{
				if (errorMessages)
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the vertex shader from the file " << i_path
						<< ":\n" << reinterpret_cast<char*>(errorMessages->GetBufferPointer());
					DEBUG_PRINT(errorMessage.str());
					errorMessages->Release();
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the vertex shader from the file " << i_path;
					DEBUG_PRINT(errorMessage.str());
				}
				return false;
			}
		}
		// Create the vertex shader object
		bool wereThereErrors = false;
		{
			HRESULT result = eae6320::Graphics::get_direct3dDevice()->CreateVertexShader(reinterpret_cast<DWORD*>(compiledShader->GetBufferPointer()),
				&o_vertexShader);
			if (FAILED(result))
			{
				DEBUG_PRINT("Direct3D failed to create the vertex shader");
				wereThereErrors = true;
			}
			compiledShader->Release();
		}
		return !wereThereErrors;
	}
}
