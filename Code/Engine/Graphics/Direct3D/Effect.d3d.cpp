
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

#include "../Texture.h"
#include "../Context.h"
#include "../../System/UserOutput.h"

namespace
{
	bool LoadFragmentShader(const Lame::Context *i_context, std::string i_path, IDirect3DPixelShader9*& o_fragmentShader, ID3DXConstantTable*& o_fragmentConstantTable);
	bool LoadVertexShader(const Lame::Context *i_context, std::string i_path, IDirect3DVertexShader9*& o_vertexShader, ID3DXConstantTable*& o_vertexConstantTable);
}

namespace Lame
{
	Effect* Effect::Create(std::shared_ptr<Context> i_context, const char* i_vertex_path, const char* i_fragment_path, Lame::EnumMask<RenderState> i_renderMask)
	{
		IDirect3DVertexShader9* vertexShader = nullptr;
		ID3DXConstantTable *vertexConstantTable = nullptr;

		IDirect3DPixelShader9* fragmentShader = nullptr;
		ID3DXConstantTable *fragmentConstantTable = nullptr;

		if (!LoadFragmentShader(i_context.get(), i_fragment_path, fragmentShader, fragmentConstantTable) || 
			!LoadVertexShader(i_context.get(), i_vertex_path, vertexShader, vertexConstantTable))
			return nullptr;

		Effect *effect = new Effect(i_context, i_renderMask);
		if (effect)
		{
			effect->vertexShader = vertexShader;
			effect->fragmentShader = fragmentShader;
			effect->vertexConstantTable = vertexConstantTable;
			effect->fragmentConstantTable = fragmentConstantTable;
		}
		else
		{
			Lame::UserOutput::Display("Failed to create Effect, due to insufficient memory.", "Effect Loading Error");
		}
		return effect;
	}

	bool Effect::Bind()
	{
		if (!context)
		{
			Lame::UserOutput::Display("Direct3D Context has been destroyed, failed to bind Effect.", "Effect bind failure");
			return false;
		}

		bool success = true;
		HRESULT result;

		// Set the vertex and fragment shaders
		result = context->get_direct3dDevice()->SetVertexShader(vertexShader);
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
			success = success && SUCCEEDED(result);
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

		//wireframe mode
		if (is_wireframe())
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		else
			result = context->get_direct3dDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		success = success && SUCCEEDED(result);

		return success;
	}

	Effect::~Effect()
	{
		if (!context)
		{
			Lame::UserOutput::Display("Direct3D Context has been destroyed before effect.", "WARNING: Effect destruction after context");
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
		ID3DXConstantTable *constantTable = get_constant_table(i_shader);
		D3DXHANDLE handle = constantTable->GetConstantByName(nullptr, i_constant.c_str());
		if (handle != nullptr)
		{
			o_constantId = std::make_tuple(handle, static_cast<DWORD>(constantTable->GetSamplerIndex(handle)));
			return true;
		}
		else
			return false;
	}

	bool Effect::SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const Lame::Matrix4x4 &i_val)
	{
		HRESULT result = get_constant_table(i_shader)->SetMatrixTranspose(context->get_direct3dDevice(), std::get<0>(i_constant), reinterpret_cast<const D3DXMATRIX*>(&i_val));
		if (FAILED(result))
		{
			Lame::UserOutput::Display("DirectX failed to set a constant uniform Matrix value.");
			return false;
		}
		return true;
	}

	bool Effect::SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const float *i_val, const size_t &i_val_count)
	{
		HRESULT result = get_constant_table(i_shader)->SetFloatArray(context->get_direct3dDevice(), std::get<0>(i_constant), i_val, static_cast<UINT>(i_val_count));
		if (FAILED(result))
		{
			Lame::UserOutput::Display("DirectX failed to set a constant uniform float array value.");
			return false;
		}
		return true;
	}

	bool Effect::SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const Lame::Texture *i_val)
	{
		HRESULT result = context->get_direct3dDevice()->SetTexture(std::get<1>(i_constant), i_val->texture());
		if (FAILED(result))
		{
			Lame::UserOutput::Display("DirectX failed to set a constant uniform Texture value.");
			return false;
		}
		return true;
	}
}

namespace
{
	bool LoadFragmentShader(const Lame::Context *i_context, std::string i_path, IDirect3DPixelShader9*& o_fragmentShader, ID3DXConstantTable*& o_fragmentConstantTable)
	{
		char const * const errorHeader = "Fragment Shader Loading Error";

		// Load the source code from file and compile it
		char* compiledShader = Lame::File::LoadBinary(i_path);
		if (!compiledShader)
			return false;

		// Create the fragment shader object
		HRESULT result = i_context->get_direct3dDevice()->CreatePixelShader(reinterpret_cast<DWORD*>(compiledShader),
			&o_fragmentShader);
		if (FAILED(result) || !o_fragmentShader)
		{
			Lame::UserOutput::Display("Direct3D failed to create the fragment shader", errorHeader);
			delete[] compiledShader;
			return false;
		}

		//get the constant table
		result = D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(compiledShader), &o_fragmentConstantTable);
		if (FAILED(result) || !o_fragmentConstantTable)
		{
			Lame::UserOutput::Display("Direct3D failed to load the Fragment Constant Table", errorHeader);
			delete[] compiledShader;
			return false;
		}
		delete[] compiledShader;
		return true;
	}

	bool LoadVertexShader(const Lame::Context *i_context, std::string i_path, IDirect3DVertexShader9*& o_vertexShader, ID3DXConstantTable*& o_vertexConstantTable)
	{
		char const * const errorHeader = "Vertex Shader Loading Error";

		// Load the source code from file and compile it
		char* compiledShader = Lame::File::LoadBinary(i_path);
		if (!compiledShader)
			return false;

		// Create the vertex shader object
		HRESULT result = i_context->get_direct3dDevice()->CreateVertexShader(reinterpret_cast<DWORD*>(compiledShader),
			&o_vertexShader);
		if (FAILED(result) || !o_vertexShader)
		{
			Lame::UserOutput::Display("Direct3D failed to create the vertex shader", errorHeader);
			delete[] compiledShader;
			return false;
		}

		//get the constant table
		result = D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(compiledShader), &o_vertexConstantTable);
		if (FAILED(result) || !o_vertexConstantTable)
		{
			Lame::UserOutput::Display("Direct3D failed to load the Vertex Constant Table", errorHeader);
			delete[] compiledShader;
			return false;
		}

		delete[] compiledShader;
		return true;
	}
}
