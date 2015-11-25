
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
	Effect* Effect::Create(std::shared_ptr<Context> i_context, const char* i_vertex_path, const char* i_fragment_path, Engine::EnumMask<RenderState> i_renderMask)
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

		Effect *effect = new Effect(i_context, i_renderMask);
		if (effect)
		{
			effect->vertexShader = vertexShader;
			effect->fragmentShader = fragmentShader;
			effect->vertexConstantTable = vertexConstantTable;
			effect->fragmentConstantTable = fragmentConstantTable;

			if (!effect->CacheConstant(Shader::Vertex, LocalToWorldUniformName, effect->localToWorldUniformId) ||
				!effect->CacheConstant(Shader::Vertex, WorldToViewUniformName, effect->worldToViewUniformId) ||
				!effect->CacheConstant(Shader::Vertex, ViewToScreenUniformName, effect->viewToScreenUniformId))
			{
				System::UserOutput::Display("DirectX failed to find all required uniform constants for effect.");
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

		bool success = true;

		// Set the vertex and fragment shaders
		HRESULT result = context->get_direct3dDevice()->SetVertexShader(vertexShader);
		success = success && SUCCEEDED(result);
		result = context->get_direct3dDevice()->SetPixelShader(fragmentShader);
		success = success && SUCCEEDED(result);

		//alpha transparency
		if (has_transparency())
		{
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			success = success && SUCCEEDED(result);
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			success = success && SUCCEEDED(result);
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		success = success && SUCCEEDED(result);

		//face culling
		if (has_face_cull())
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		else
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		success = success && SUCCEEDED(result);

		//depth testing
		if (has_depth_test())
		{
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		}
		else
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		success = success && SUCCEEDED(result);

		//depth writing
		if (has_depth_write())
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		else
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		success = success && SUCCEEDED(result);

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

	bool Effect::CacheConstant(const Shader &i_shader, const std::string &i_constant, ConstantHandle &o_constantId)
	{
		o_constantId = get_constant_table(i_shader)->GetConstantByName(nullptr, i_constant.c_str());
		return o_constantId != nullptr;
	}

	bool Effect::SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const eae6320::Math::cMatrix_transformation &i_val)
	{
		HRESULT result = get_constant_table(i_shader)->SetMatrixTranspose(context->get_direct3dDevice(), i_constant, reinterpret_cast<const D3DXMATRIX*>(&i_val));
		if (!SUCCEEDED(result))
		{
			System::UserOutput::Display("DirectX failed to set a constant uniform value.");
			return false;
		}
		return true;
	}

	bool Effect::SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const float *i_val, const size_t &i_val_count)
	{
		HRESULT result = get_constant_table(i_shader)->SetFloatArray(context->get_direct3dDevice(), i_constant, i_val, static_cast<UINT>(i_val_count));
		if (!SUCCEEDED(result))
		{
			System::UserOutput::Display("DirectX failed to set a constant uniform value.");
			return false;
		}
		return true;
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
