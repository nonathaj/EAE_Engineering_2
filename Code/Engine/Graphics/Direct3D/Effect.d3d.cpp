
#include "../Effect.h"

#include "../../Windows/Includes.h"
#include "../../Windows/Functions.h"

#include <d3dx9shader.h>

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <sstream>
#include "../../System/FileLoader.h"

#include "../Context.h"
#include "../../System/UserOutput.h"

namespace
{
	bool LoadFragmentShader(const Lame::Context *i_context, std::string i_path, IDirect3DPixelShader9*& o_fragmentShader, ID3DXConstantTable** o_fragmentConstantTable);
	bool LoadVertexShader(const Lame::Context *i_context, std::string i_path, IDirect3DVertexShader9*& o_vertexShader, ID3DXConstantTable** o_vertexConstantTable);
}

namespace Lame
{
	Effect* Effect::Create(std::shared_ptr<Context> i_context, const char* i_vertex_path, const char* i_fragment_path)
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

		ID3DXConstantTable *vertexConstantTable = nullptr;
		ID3DXConstantTable *fragmentConstantTable = nullptr;

		if (!LoadFragmentShader(i_context.get(), i_fragment_path, fragmentShader, &fragmentConstantTable) || 
			!LoadVertexShader(i_context.get(), i_vertex_path, vertexShader, &vertexConstantTable))
			return nullptr;

		Effect *effect = new Effect(i_context);
		if (effect)
		{
			effect->vertexShader = vertexShader;
			effect->fragmentShader = fragmentShader;
			effect->vertexConstantTable = vertexConstantTable;
			effect->fragmentConstantTable = fragmentConstantTable;

			if (!effect->CacheConstant(PositionUniformName))
			{
				System::UserOutput::Display("Failed to find Position uniform constant");
				delete effect;
				return nullptr;
			}
		}
		else
		{
			System::UserOutput::Display("Failed to create Effect, due to insufficient memory.", "Effect Loading Error");
		}
		return effect;
	}

	bool Effect::Bind()
	{
		if (!context)
		{
			System::UserOutput::Display("Direct3D Context has been destroyed, failed to bind Effect.", "Effect bind failure");
			return false;
		}

		HRESULT result = context->get_direct3dDevice()->SetVertexShader(vertexShader);
		bool success = SUCCEEDED(result);
		assert(success);
		result = context->get_direct3dDevice()->SetPixelShader(fragmentShader);
		success = success && SUCCEEDED(result);
		assert(success);
		return success;
	}

	Effect::~Effect()
	{
		if (!context)
		{
			System::UserOutput::Display("Direct3D Context has been destroyed before effect.", "WARNING: Effect destruction after context");
		}

		if (vertexConstantTable)
		{
			vertexConstantTable->Release();
			vertexConstantTable = nullptr;
		}
		if (fragmentConstantTable)
		{
			fragmentConstantTable->Release();
			fragmentConstantTable = nullptr;
		}

		if (vertexShader)
		{
			vertexShader->Release();
			vertexShader = nullptr;
		}
		if (fragmentShader)
		{
			fragmentShader->Release();
			fragmentShader = nullptr;
		}
	}

	bool Effect::CacheConstant(const std::string &i_constant, Engine::HashedString* o_constantId)
	{
		Engine::HashedString hashed(i_constant.c_str());
		if (o_constantId)
			*o_constantId = hashed;

		//if we already have this constant cached, we already successfully cached it
		if (constants.find(hashed) != constants.end())
			return true;

		D3DXHANDLE handle = vertexConstantTable->GetConstantByName(nullptr, i_constant.c_str());
		if (!handle)
			handle = fragmentConstantTable->GetConstantByName(nullptr, i_constant.c_str());

		if (handle)
			constants[hashed] = handle;
		return handle != nullptr;
	}

	bool Effect::SetConstant(const Engine::HashedString &i_constant, const eae6320::Math::cVector &i_val)
	{
		auto itr = constants.find(i_constant);
		if (itr == constants.end())					//fail if we don't have a cache'd version of this constant
			return false;

		D3DXHANDLE handle = itr->second;
		float floatArray[] = { i_val.x, i_val.y, i_val.z };
		HRESULT result = vertexConstantTable->SetFloatArray(context->get_direct3dDevice(), handle, floatArray, 2);
		if (!SUCCEEDED(result))
			result = fragmentConstantTable->SetFloatArray(context->get_direct3dDevice(), handle, floatArray, 2);

		if (!SUCCEEDED(result))
		{
			System::UserOutput::Display("DirectX failed to set a constant uniform value.");
			return false;
		}
		else
		{
			return true;
		}
	}
}

namespace
{
	bool LoadFragmentShader(const Lame::Context *i_context, std::string i_path, IDirect3DPixelShader9*& o_fragmentShader, ID3DXConstantTable** o_fragmentConstantTable)
	{
		char const * const errorHeader = "Fragment Shader Loading Error";
		bool wereThereErrors = false;

		// Load the source code from file and compile it
		char* compiledShader = System::File::LoadBinary(i_path);
		if (!compiledShader)
			wereThereErrors = true;
		else
		{
			// Create the fragment shader object
			HRESULT result = i_context->get_direct3dDevice()->CreatePixelShader(reinterpret_cast<DWORD*>(compiledShader),
				&o_fragmentShader);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to create the fragment shader", errorHeader);
				wereThereErrors = true;
			}

			//get the constant table
			result = D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(compiledShader), o_fragmentConstantTable);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to load the Fragment Constant Table", errorHeader);
				wereThereErrors = true;
			}
		}
		delete[] compiledShader;
		return !wereThereErrors;
	}

	bool LoadVertexShader(const Lame::Context *i_context, std::string i_path, IDirect3DVertexShader9*& o_vertexShader, ID3DXConstantTable** o_vertexConstantTable)
	{
		char const * const errorHeader = "Vertex Shader Loading Error";
		bool wereThereErrors = false;

		// Load the source code from file and compile it
		char* compiledShader = System::File::LoadBinary(i_path);
		if (!compiledShader)
			wereThereErrors = true;
		else
		{
			// Create the vertex shader object
			HRESULT result = i_context->get_direct3dDevice()->CreateVertexShader(reinterpret_cast<DWORD*>(compiledShader),
				&o_vertexShader);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to create the vertex shader", errorHeader);
				wereThereErrors = true;
			}

			//get the constant table
			result = D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(compiledShader), o_vertexConstantTable);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to load the Vertex Constant Table", errorHeader);
				wereThereErrors = true;
			}
		}

		delete[] compiledShader;
		return !wereThereErrors;
	}
}
