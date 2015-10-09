
#include "../Effect.h"

#include "../../Windows/Includes.h"
#include "../../Windows/Functions.h"

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sstream>
#include <d3dx9shader.h>

#include "../Context.h"
#include "../../System/UserOutput.h"

namespace
{
	bool LoadFragmentShader(const Lame::Context *i_context, std::string i_path, IDirect3DPixelShader9*& o_fragmentShader);
	bool LoadVertexShader(const Lame::Context *i_context, std::string i_path, IDirect3DVertexShader9*& o_vertexShader);
}

namespace Lame
{
	Effect* Effect::Create(const Context *i_context, std::string i_vertex_path, std::string i_fragment_path)
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

		if (!LoadFragmentShader(i_context, i_fragment_path, fragmentShader) || !LoadVertexShader(i_context, i_vertex_path, vertexShader))
			return nullptr;

		Effect *effect = new Effect();
		if (effect)
		{
			effect->vertexShader = vertexShader;
			effect->pixelShader = fragmentShader;
		}
		else
		{
			System::UserOutput::Display("Failed to create Effect, due to insufficient memory.", "Effect Loading Error");
		}
		return effect;
	}

	bool Effect::Bind(const Context *i_context)
	{
		HRESULT result = i_context->get_direct3dDevice()->SetVertexShader(vertexShader);
		bool success = SUCCEEDED(result);
		assert(success);
		result = i_context->get_direct3dDevice()->SetPixelShader(pixelShader);
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
	bool LoadFragmentShader(const Lame::Context *i_context, std::string i_path, IDirect3DPixelShader9*& o_fragmentShader)
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
					System::UserOutput::Display(errorMessage.str());
					errorMessages->Release();
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the fragment shader from the file " << i_path;
					System::UserOutput::Display(errorMessage.str());
				}
				return false;
			}
		}
		// Create the fragment shader object
		bool wereThereErrors = false;
		{
			HRESULT result = i_context->get_direct3dDevice()->CreatePixelShader(reinterpret_cast<DWORD*>(compiledShader->GetBufferPointer()),
				&o_fragmentShader);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to create the fragment shader");
				wereThereErrors = true;
			}
			compiledShader->Release();
		}
		return !wereThereErrors;
	}

	bool LoadVertexShader(const Lame::Context *i_context, std::string i_path, IDirect3DVertexShader9*& o_vertexShader)
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
					System::UserOutput::Display(errorMessage.str());
					errorMessages->Release();
				}
				else
				{
					std::stringstream errorMessage;
					errorMessage << "Direct3D failed to compile the vertex shader from the file " << i_path;
					System::UserOutput::Display(errorMessage.str());
				}
				return false;
			}
		}
		// Create the vertex shader object
		bool wereThereErrors = false;
		{
			HRESULT result = i_context->get_direct3dDevice()->CreateVertexShader(reinterpret_cast<DWORD*>(compiledShader->GetBufferPointer()),
				&o_vertexShader);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to create the vertex shader");
				wereThereErrors = true;
			}
			compiledShader->Release();
		}
		return !wereThereErrors;
	}
}
