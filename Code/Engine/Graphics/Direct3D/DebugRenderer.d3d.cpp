
#include "../DebugRenderer.h"

#ifdef ENABLE_DEBUG_RENDERING

#include "../Vertex.h"
#include "../Effect.h"
#include "../Context.h"
#include "../Mesh.h"
#include "../../Core/Matrix4x4.h"
#include "../../System/UserOutput.h"
#include "../../Core/Quaternion.h"

namespace Lame
{
	DebugRenderer* DebugRenderer::Create(std::shared_ptr<Lame::Context> i_context, const size_t i_line_count)
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
			// Our code will only ever write to the buffer
			usage |= D3DUSAGE_WRITEONLY;
		}

		//Create the Vertex Buffer
		IDirect3DVertexBuffer9 *vertex_buffer;
		{
			const UINT bufferSize = static_cast<UINT>(i_line_count * sizeof(Vertex));
			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL pool = D3DPOOL_DEFAULT;
			HANDLE* const notUsed = nullptr;
			const HRESULT result = i_context->get_direct3dDevice()->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, pool, &vertex_buffer, notUsed);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to create a vertex buffer");
				return nullptr;
			}
		}

		std::shared_ptr<Lame::Effect> line_effect(Lame::Effect::Create(i_context, "data/debug/line.effect.bin", false));
		std::shared_ptr<Lame::Effect> shape_effect(Lame::Effect::Create(i_context, "data/debug/shape.effect.bin", true));
		if (!line_effect || !shape_effect)
		{
			vertex_buffer->Release();
			System::UserOutput::Display("Failed to Create Debug Effects");
			return nullptr;
		}

		DebugRenderer* deb = new DebugRenderer();
		if (deb)
		{
			deb->max_lines_count = i_line_count;
			deb->line_effect = line_effect;
			deb->shape_effect = shape_effect;
			deb->vertex_buffer_ = vertex_buffer;
			deb->line_vertices.reserve(i_line_count);
			return deb;
		}
		else
		{
			vertex_buffer->Release();
			System::UserOutput::Display("Failed to Create DebugRenderer");
			return nullptr;
		}
	}

	bool DebugRenderer::RenderLines(const Engine::Matrix4x4& i_worldToView, const Engine::Matrix4x4& i_viewToScreen)
	{
		// Lock the vertex buffer for editing
		Vertex *vertexData;
		HRESULT result = vertex_buffer_->Lock(0, 0, reinterpret_cast<void**>(&vertexData), 0);
		if (FAILED(result))
		{
			System::UserOutput::Display("Direct3D failed to lock the vertex buffer");
			return false;
		}

		//Fill the buffer
		memcpy(vertexData, line_vertices.data(), line_vertices.size() * sizeof(*line_vertices.data()));
		//std::copy(line_vertices.begin(), line_vertices.end(), vertexData);

		// Unlock the vertex buffer
		result = vertex_buffer_->Unlock();
		if (FAILED(result))
		{
			System::UserOutput::Display("Direct3D failed to unlock the vertex buffer");
			return false;
		}

		//set the effect
		if (!line_effect->Bind() ||
			!line_effect->SetWorldToView(i_worldToView) ||
			!line_effect->SetViewToScreen(i_viewToScreen))
		{
			return false;
		}

		// Bind a specific vertex buffer to the device as a data source
		{
			// There can be multiple streams of data feeding the display adaptor at the same time
			const unsigned int streamIndex = 0;
			// It's possible to start streaming data in the middle of a vertex buffer
			const unsigned int bufferOffset = 0;
			// The "stride" defines how large a single vertex is in the stream of data
			const unsigned int bufferStride = sizeof(Vertex);
			result = line_effect->get_context()->get_direct3dDevice()->SetStreamSource(streamIndex, vertex_buffer_, bufferOffset, bufferStride);
			if (FAILED(result))
			{
				return false;
			}
		}

		result = line_effect->get_context()->get_direct3dDevice()->DrawPrimitive(D3DPT_LINELIST, 0, static_cast<UINT>(line_vertices.size() / 2));
		if (FAILED(result))
		{
			return false;
		}

		return true;
	}
}

#endif //ENABLE_DEBUG_RENDERING
