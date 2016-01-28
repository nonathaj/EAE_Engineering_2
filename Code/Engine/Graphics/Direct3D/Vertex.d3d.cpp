
#include "../Vertex.h"

#include "../Context.h"
#include "../../System/UserOutput.h"

namespace Lame
{
	bool Vertex::SetVertexFormat(std::shared_ptr<Lame::Context> i_context, IDirect3DVertexDeclaration9 ** o_vertex_declaration)
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

			// POSITION
			// 3 floats == 12 bytes
			// Offset = 0
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

			// TEXCOORD0
			// 2 floats == 8 bytes
			// Offset = 12
			{ 0, 12 , D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

			// COLOR0
			// D3DCOLOR == 4 bytes
			// Offset = 20
			{ 0, 20, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

			// The following marker signals the end of the vertex declaration
			D3DDECL_END()
		};
		HRESULT result = i_context->get_direct3dDevice()->CreateVertexDeclaration(vertexElements, o_vertex_declaration);
		if (SUCCEEDED(result))
		{
			result = i_context->get_direct3dDevice()->SetVertexDeclaration(*o_vertex_declaration);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to set the vertex declaration");
				return false;
			}
		}
		else
		{
			System::UserOutput::Display("Direct3D failed to create a Direct3D9 vertex declaration");
			return false;
		}
		return true;
	}
}
