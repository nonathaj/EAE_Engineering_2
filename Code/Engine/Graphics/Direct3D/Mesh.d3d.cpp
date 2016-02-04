
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

	Mesh* Mesh::CreateRightHanded(const bool i_static, std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count)
	{
		if (i_index_count % 3 != 0)		//index buffer must be a list of triangles
		{
			System::UserOutput::Display("Cannot create a Mesh with non-triangular data. (Ensure number of indices is divisible by 3)");
			return nullptr;
		}
		bool hasIndices = i_index_count > 0 && i_indices;
		if(hasIndices)
			SwapIndexOrder(i_indices, i_index_count);

		Mesh *mesh = CreateLeftHanded(i_static, i_context, i_vertices, i_vertex_count, i_indices, i_index_count);

		if (hasIndices)
			SwapIndexOrder(i_indices, i_index_count);
		return mesh;
	}

	//Create a mesh with LEFT-HANDED indices
	Mesh* Mesh::CreateLeftHanded(const bool i_static, std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count)
	{
		if (i_index_count % 3 != 0)		//index buffer must be a list of triangles
		{
			System::UserOutput::Display("Cannot create a Mesh with non-triangular data. (Ensure number of indices is divisible by 3)");
			return nullptr;
		}

		Mesh *mesh = new Mesh(i_vertex_count, i_index_count, Lame::Mesh::PrimitiveType::TriangleList, i_context);
		if (!mesh)
		{
			System::UserOutput::Display("Failed to create Mesh, due to insufficient memory.", "Mesh Loading Error");
			return nullptr;
		}

		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = i_context->GetVertexProcessingUsage(usage);
			if (FAILED(result))
			{
				System::UserOutput::Display("Unable to get vertex processing usage information");
				delete mesh;
				return nullptr;
			}
			// Our code will only ever write to the buffer
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
			{
				const UINT bufferSize = static_cast<UINT>(i_vertex_count * sizeof(Vertex));
				// We will define our own vertex format
				const DWORD useSeparateVertexDeclaration = 0;
				// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
				const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
				HANDLE* const notUsed = NULL;
				const HRESULT result = i_context->get_direct3dDevice()->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
					&mesh->vertex_buffer_, notUsed);
				if (FAILED(result))
				{
					System::UserOutput::Display("Direct3D failed to create a vertex buffer");
					delete mesh;
					return nullptr;
				}
			}

			// Fill the vertex buffer with the triangle's vertices
			if(!mesh->UpdateVertices(i_vertices))
			{
				System::UserOutput::Display("Failed to copy vertex data to the mesh");
				delete mesh;
				return nullptr;
			}
		}

		//Create the Index Buffer
		if(i_index_count > 0 && i_indices)
		{
			// Create an index buffer
			{
				// We are drawing a square
				UINT bufferSize = static_cast<UINT>(i_index_count * sizeof(uint32_t));
				// We'll use 32-bit indices in this class to keep things simple
				// (i.e. every index will be a 32 bit unsigned integer)
				const D3DFORMAT format = D3DFMT_INDEX32;
				// Place the index buffer into memory that Direct3D thinks is the most appropriate
				const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
				HANDLE* notUsed = NULL;
				const HRESULT result = i_context->get_direct3dDevice()->CreateIndexBuffer(bufferSize, usage, format, useDefaultPool,
					&mesh->index_buffer_, notUsed);
				if (FAILED(result))
				{
					System::UserOutput::Display("Direct3D failed to create an index buffer");
					delete mesh;
					return nullptr;
				}
			}
			// Fill the index buffer with the triangles' connectivity data
			if (!mesh->UpdateIndices(i_indices))
			{
				System::UserOutput::Display("Failed to copy index data to the mesh");
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

	bool Mesh::UpdateVertices(const Vertex* i_vertices)
	{
		// Fill the vertex buffer with the triangle's vertices
		// Before the vertex buffer can be changed it must be "locked"'
		Vertex *vertexData;
		{
			const unsigned int lockEntireBuffer = 0;
			const DWORD useDefaultLockingBehavior = 0;
			const HRESULT result = vertex_buffer_->Lock(lockEntireBuffer, lockEntireBuffer,
				reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
			if (FAILED(result))
			{
				return false;
			}
		}

		//Fill the buffer
		memcpy(vertexData, i_vertices, vertex_count_ * sizeof(*i_vertices));
		//Preferred, but VC++ doesn't like unchecked iterators
		//std::copy(i_vertices, i_vertices + i_vertex_count, vertexData);

		// The buffer must be "unlocked" before it can be used
		{
			const HRESULT result = vertex_buffer_->Unlock();
			if (FAILED(result))
			{
				return false;
			}
		}
		return true;
	}

	bool Mesh::UpdateIndices(const uint32_t* i_indices)
	{
		// Before the index buffer can be changed it must be "locked"
		uint32_t *indexData;
		{
			const unsigned int lockEntireBuffer = 0;
			const DWORD useDefaultLockingBehavior = 0;
			const HRESULT result = index_buffer_->Lock(lockEntireBuffer, lockEntireBuffer,
				reinterpret_cast<void**>(&indexData), useDefaultLockingBehavior);
			if (FAILED(result))
			{
				return false;
			}
		}

		// Fill the buffer
		memcpy(indexData, i_indices, index_count_ * sizeof(*i_indices));
		//Preferred, but VC++ doesn't like unchecked iterators
		//std::copy(i_indices, i_indices + i_index_count, indexData);

		// The buffer must be "unlocked" before it can be used
		{
			const HRESULT result = index_buffer_->Unlock();
			if (FAILED(result))
			{
				return false;
			}
		}
		return true;
	}

	bool Mesh::Draw() const
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
			if (!SUCCEEDED(result))
				return false;
		}

		// Bind a specific index buffer to the device as a data source
		{
			result = context->get_direct3dDevice()->SetIndices(index_buffer_);
			if (!SUCCEEDED(result))
				return false;
		}
		// Render objects from the current streams
		{
			const D3DPRIMITIVETYPE primitiveType = GetD3DPrimitiveType(primitive_type());
			if (index_count_ > 0)
			{
				const UINT primitiveCount = static_cast<UINT>(GetPrimitiveCount(primitive_type(), index_count_));
				result = context->get_direct3dDevice()->DrawIndexedPrimitive(primitiveType,
					0, 0, static_cast<UINT>(vertex_count_), 0, primitiveCount);
			}
			else
			{
				const UINT primitiveCount = static_cast<UINT>(GetPrimitiveCount(primitive_type(), vertex_count_));
				result = context->get_direct3dDevice()->DrawPrimitive(primitiveType, 0, primitiveCount);
			}
			return SUCCEEDED(result);
		}
	}
}
