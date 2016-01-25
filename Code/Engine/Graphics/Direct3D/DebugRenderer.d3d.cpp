
#include "../DebugRenderer.h"

#include "../Vertex.h"
#include "../Effect.h"
#include "../Context.h"
#include "../../System/UserOutput.h"

namespace Lame
{
	DebugRenderer* DebugRenderer::Create(std::shared_ptr<Lame::Effect> i_effect, const size_t i_line_count)
	{
		IDirect3DVertexBuffer9 *vertex_buffer;

		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = i_effect->get_context()->GetVertexProcessingUsage(usage);
			if (FAILED(result))
			{
				System::UserOutput::Display("Unable to get vertex processing usage information");
				return nullptr;
			}
			// Our code will only ever write to the buffer
			usage |= D3DUSAGE_WRITEONLY;
		}

		//Create the Vertex Buffer
		{
			const UINT bufferSize = static_cast<UINT>(i_line_count * sizeof(Vertex));
			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL pool = D3DPOOL_DEFAULT;
			HANDLE* const notUsed = nullptr;
			const HRESULT result = i_effect->get_context()->get_direct3dDevice()->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, pool, &vertex_buffer, notUsed);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to create a vertex buffer");
				return nullptr;
			}
		}

		DebugRenderer* deb = new DebugRenderer();
		if (deb)
		{
			deb->max_lines_count = i_line_count;
			deb->effect = i_effect;
			deb->vertex_buffer_ = vertex_buffer;
			deb->line_vertices.reserve(i_line_count);
			return deb;
		}
		else
		{
			if (vertex_buffer)
				vertex_buffer->Release();
			System::UserOutput::Display("Failed to Create DebugRenderer");
			return nullptr;
		}
	}

	bool DebugRenderer::AddLine(const Engine::Vector3& i_start, const Engine::Vector3& i_end, const Lame::Color32& i_color)
	{
		if (line_vertices.size() >= max_lines_count)
			return false;

		Lame::Vertex start;
		start.position = i_start;
		start.color = i_color;
		line_vertices.push_back(start);

		Lame::Vertex end;
		end.position = i_end;
		end.color = i_color;
		line_vertices.push_back(end);

		return true;
	}

	bool DebugRenderer::Render()
	{
		// Lock the vertex buffer for editing
		Vertex *vertexData;
		HRESULT result = vertex_buffer_->Lock(0, 0, reinterpret_cast<void**>(&vertexData), 0);
		if (FAILED(result))
		{
			System::UserOutput::Display("Direct3D failed to lock the vertex buffer");
			line_vertices.clear();
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
			line_vertices.clear();
			return false;
		}

		//set the effect
		if (!effect->Bind())
		{
			line_vertices.clear();
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
			result = effect->get_context()->get_direct3dDevice()->SetStreamSource(streamIndex, vertex_buffer_, bufferOffset, bufferStride);
			if (FAILED(result))
			{
				line_vertices.clear();
				return false;
			}
		}

		result = effect->get_context()->get_direct3dDevice()->DrawPrimitive(D3DPT_LINELIST, 0, static_cast<UINT>(line_vertices.size()));
		if (FAILED(result))
		{
			line_vertices.clear();
			return false;
		}

		//clear the data
		line_vertices.clear();
		return true;
	}
}