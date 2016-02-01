
#include "../Sprite.h"

#include "../Vertex.h"
#include "../Context.h"
#include "../Texture.h"

#include "../../System/UserOutput.h"

namespace Lame
{
	Sprite* Sprite::Create(std::shared_ptr<Context> i_context, const Engine::Rectangle2D& i_screen_pos)
	{
		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = i_context->GetVertexProcessingUsage(usage);
			if (FAILED(result))
			{
				System::UserOutput::Display("Unable to get vertex processing usage information");
				return nullptr;
			}
			usage |= D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		}

		//Create the Vertex Buffer
		IDirect3DVertexBuffer9 *vertex_buffer;
		IDirect3DVertexDeclaration9 *vertex_declaration;
		{
			// Initialize the vertex format
			if (!Lame::Vertex::SetVertexFormat(i_context, &vertex_declaration))
			{
				return nullptr;
			}

			// Create a vertex buffer
			{
				const UINT bufferSize = static_cast<UINT>(4 * sizeof(Vertex));
				// We will define our own vertex format
				const DWORD useSeparateVertexDeclaration = 0;
				// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
				const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
				HANDLE* const notUsed = NULL;
				const HRESULT result = i_context->get_direct3dDevice()->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
					&vertex_buffer, notUsed);
				if (FAILED(result))
				{
					System::UserOutput::Display("Direct3D failed to create a vertex buffer");
					return nullptr;
				}
			}
			// Fill the vertex buffer with the triangle's vertices
			{
				// Before the vertex buffer can be changed it must be "locked"'
				Vertex *vertexData;
				{
					const unsigned int lockEntireBuffer = 0;
					const DWORD useDefaultLockingBehavior = 0;
					const HRESULT result = vertex_buffer->Lock(lockEntireBuffer, lockEntireBuffer,
						reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
					if (FAILED(result))
					{
						System::UserOutput::Display("Direct3D failed to lock the vertex buffer");
						return nullptr;
					}
				}
				//Fill the buffer
				{
					memcpy(vertexData, i_vertices, i_vertex_count * sizeof(*i_vertices));

					//Preferred, but VC++ doesn't like unchecked iterators
					//std::copy(i_vertices, i_vertices + i_vertex_count, vertexData);
				}
				// The buffer must be "unlocked" before it can be used
				{
					const HRESULT result = vertex_buffer->Unlock();
					if (FAILED(result))
					{
						System::UserOutput::Display("Direct3D failed to unlock the vertex buffer");
						return nullptr;
					}
				}
			}
		}
	}

	bool Sprite::Render()
	{
		// Bind a specific vertex buffer to the device as a data source
		{
			// There can be multiple streams of data feeding the display adaptor at the same time
			const unsigned int streamIndex = 0;
			// It's possible to start streaming data in the middle of a vertex buffer
			const unsigned int bufferOffset = 0;
			// The "stride" defines how large a single vertex is in the stream of data
			const unsigned int bufferStride = sizeof(Vertex);
			HRESULT result = context()->get_direct3dDevice()->SetStreamSource(streamIndex, vertex_buffer_, bufferOffset, bufferStride);
			if (!SUCCEEDED(result))
				return false;
		}

		//Render the Quad
		{
			HRESULT result = context()->get_direct3dDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
			return SUCCEEDED(result);
		}
	}
}