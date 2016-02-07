
#include "../Mesh.h"

#include <string>
#include <sstream>
#include <d3d9.h>
#include <algorithm>
#include <iterator>

#include "../Context.h"
#include "../Vertex.h"
#include "../Graphics.h"
#include "../../System/UserOutput.h"

namespace
{
	D3DPRIMITIVETYPE GetD3DPrimitiveType(Lame::Mesh::PrimitiveType i_prim)
	{
		switch (i_prim)
		{
		case Lame::Mesh::PrimitiveType::TriangleList: return D3DPT_TRIANGLELIST;
		case Lame::Mesh::PrimitiveType::TriangleStrip: return D3DPT_TRIANGLESTRIP;
		case Lame::Mesh::PrimitiveType::TriangleFan: return D3DPT_TRIANGLEFAN;
		case Lame::Mesh::PrimitiveType::LineList: return D3DPT_LINELIST;
		case Lame::Mesh::PrimitiveType::LineStrip: return D3DPT_LINESTRIP;
		default: return static_cast<D3DPRIMITIVETYPE>(0);
		}
	}
}

namespace Lame
{
	Mesh::Mesh(size_t i_vertex_count, size_t i_index_count, PrimitiveType i_prim_type, std::shared_ptr<Context> i_context) :
		vertex_count_(i_vertex_count),
		index_count_(i_index_count),
		context(i_context),
		primitive_type_(i_prim_type),
		vertex_buffer_(nullptr),
		index_buffer_(nullptr),
		vertex_declaration_(nullptr)
	{ }

	Mesh::~Mesh()
	{
		if (vertex_buffer_)
		{
			vertex_buffer_->Release();
			vertex_buffer_ = nullptr;
		}
		if (index_buffer_)
		{
			index_buffer_->Release();
			index_buffer_ = nullptr;
		}
		if (vertex_declaration_)
		{
			context->get_direct3dDevice()->SetVertexDeclaration(nullptr);
			vertex_declaration_->Release();
			vertex_declaration_ = nullptr;
		}
	}

	Mesh* Mesh::CreateEmpty(const bool i_static, std::shared_ptr<Context> i_context, PrimitiveType i_prim_type, const size_t i_vertex_count, const size_t i_index_count)
	{
		Mesh *mesh = new Mesh(i_vertex_count, i_index_count, i_prim_type, i_context);
		if (!mesh)
		{
			Lame::UserOutput::Display("Failed to create Mesh, due to insufficient memory.", "Mesh Loading Error");
			return nullptr;
		}

		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = i_context->GetVertexProcessingUsage(usage);
			if (FAILED(result))
			{
				Lame::UserOutput::Display("Unable to get vertex processing usage information");
				delete mesh;
				return nullptr;
			}
			usage |= (i_static ? 0x0 : D3DUSAGE_DYNAMIC) | D3DUSAGE_WRITEONLY;
		}

		//Create the Vertex Buffer
		{
			// Initialize the vertex format
			if (!Lame::Vertex::SetVertexFormat(i_context, &mesh->vertex_declaration_))
			{
				delete mesh;
				return nullptr;
			}

			// Create a vertex buffer
			const UINT bufferSize = static_cast<UINT>(i_vertex_count * sizeof(Vertex));
			const HRESULT result = i_context->get_direct3dDevice()->CreateVertexBuffer(
				bufferSize, usage, 0, D3DPOOL_DEFAULT, &mesh->vertex_buffer_, nullptr);
			if (FAILED(result))
			{
				Lame::UserOutput::Display("Direct3D failed to create a vertex buffer");
				delete mesh;
				return nullptr;
			}
		}

		//Create the Index Buffer
		if (i_index_count > 0)
		{
			UINT bufferSize = static_cast<UINT>(i_index_count * sizeof(uint32_t));
			const HRESULT result = i_context->get_direct3dDevice()->CreateIndexBuffer(
				bufferSize, usage, D3DFMT_INDEX32, D3DPOOL_DEFAULT, &mesh->index_buffer_, nullptr);
			if (FAILED(result))
			{
				Lame::UserOutput::Display("Direct3D failed to create an index buffer");
				delete mesh;
				return nullptr;
			}
		}
		else
		{
			mesh->index_buffer_ = nullptr;
		}

		return mesh;
	}

	Mesh* Mesh::CreateRightHandedTriList(const bool i_static, std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count)
	{
		if (i_index_count % 3 != 0)		//index buffer must be a list of triangles
		{
			Lame::UserOutput::Display("Cannot create a TriList Mesh with non-triangular data. (Ensure number of indices is divisible by 3)");
			return nullptr;
		}
		bool hasIndices = i_index_count > 0 && i_indices;
		if(hasIndices)
			SwapIndexOrder(i_indices, i_index_count);

		Mesh *mesh = CreateLeftHandedTriList(i_static, i_context, i_vertices, i_vertex_count, i_indices, i_index_count);

		if (hasIndices)
			SwapIndexOrder(i_indices, i_index_count);
		return mesh;
	}

	//Create a mesh with LEFT-HANDED indices
	Mesh* Mesh::CreateLeftHandedTriList(const bool i_static, std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count)
	{
		if (i_index_count % 3 != 0)		//index buffer must be a list of triangles
		{
			Lame::UserOutput::Display("Cannot create a TriList Mesh with non-triangular data. (Ensure number of indices is divisible by 3)");
			return nullptr;
		}
		Mesh* mesh = CreateEmpty(i_static, i_context, Lame::Mesh::PrimitiveType::TriangleList, i_vertex_count, i_index_count);
		if (!mesh)
			return nullptr;

		if (!mesh->UpdateVertices(i_vertices))
		{
			Lame::UserOutput::Display("Failed to copy vertex data to the mesh");
			delete mesh;
			return nullptr;
		}
		if (i_indices && !mesh->UpdateIndices(i_indices))
		{
			Lame::UserOutput::Display("Failed to copy index data to the mesh");
			delete mesh;
			return nullptr;
		}
		return mesh;
	}

	bool Mesh::UpdateVertices(const Vertex* i_vertices, const size_t i_amount)
	{
		Vertex *vertexData;
		HRESULT result = vertex_buffer_->Lock(0, 0, reinterpret_cast<void**>(&vertexData), 0);
		if (FAILED(result))
			return false;

		const size_t vertsToCopy = i_amount == 0 ? vertex_count_ : i_amount;
		memcpy(vertexData, i_vertices, vertsToCopy * sizeof(*i_vertices));
		return SUCCEEDED(vertex_buffer_->Unlock());
	}

	bool Mesh::UpdateIndices(const uint32_t* i_indices, const size_t i_amount)
	{
		if (!index_buffer_)
			return false;
		// Before the index buffer can be changed it must be "locked"
		uint32_t *indexData;
		HRESULT result = index_buffer_->Lock(0, 0, reinterpret_cast<void**>(&indexData), 0);
		if (FAILED(result))
			return false;
		const size_t indsToCopy = i_amount == 0 ? index_count_ : i_amount;
		memcpy(indexData, i_indices, indsToCopy * sizeof(*i_indices));
		return SUCCEEDED(index_buffer_->Unlock());
	}

	bool Mesh::Draw(const size_t i_max_primitives) const
	{
		HRESULT result;
		// Bind a specific vertex buffer to the device as a data source
		{
			// There can be multiple streams of data feeding the display adaptor at the same time
			const unsigned int streamIndex = 0;
			// It's possible to start streaming data in the middle of a vertex buffer
			const unsigned int bufferOffset = 0;
			// The "stride" defines how large a single vertex is in the stream of data
			const unsigned int bufferStride = sizeof(Vertex);
			result = context->get_direct3dDevice()->SetStreamSource(streamIndex, vertex_buffer_, bufferOffset, bufferStride);
			if (FAILED(result))
				return false;
		}

		// Bind a specific index buffer to the device as a data source
		if(index_buffer_)
		{
			result = context->get_direct3dDevice()->SetIndices(index_buffer_);
			if (FAILED(result))
				return false;
		}
		// Render objects from the current streams
		{
			const D3DPRIMITIVETYPE primitiveType = GetD3DPrimitiveType(primitive_type());
			if (index_count_ > 0)
			{
				UINT primitiveCount = static_cast<UINT>(GetPrimitiveCount(primitive_type(), index_count_));
				if (i_max_primitives > 0)
					primitiveCount = i_max_primitives < primitiveCount ? static_cast<UINT>(i_max_primitives) : primitiveCount;
				result = context->get_direct3dDevice()->DrawIndexedPrimitive(primitiveType,
					0, 0, static_cast<UINT>(vertex_count_), 0, primitiveCount);
			}
			else
			{
				UINT primitiveCount = static_cast<UINT>(GetPrimitiveCount(primitive_type(), vertex_count_));
				if (i_max_primitives > 0)
					primitiveCount = i_max_primitives < primitiveCount ? static_cast<UINT>(i_max_primitives) : primitiveCount;
				result = context->get_direct3dDevice()->DrawPrimitive(primitiveType, 0, primitiveCount);
			}
			return SUCCEEDED(result);
		}
	}
}
