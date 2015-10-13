
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
#include "../../../External/OpenGlExtensions/OpenGlExtensions.h"

namespace Lame
{
	namespace
	{
		HRESULT GetVertexProcessingUsage(IDirect3DDevice9* i_device, DWORD& o_usage)
		{
			D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
			const HRESULT result = i_device->GetCreationParameters(&deviceCreationParameters);
			if (SUCCEEDED(result))
			{
				DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
					(D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
				o_usage = (vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
			}
			else
			{
				System::UserOutput::Display("Direct3D failed to get the device's creation parameters");
			}
			return result;
		}
	}

	Mesh::Mesh(size_t i_vertex_count, size_t i_index_count, Context *&i_context) :
		vertex_count_(i_vertex_count),
		index_count_(i_index_count),
		context(i_context),
		vertex_buffer_(nullptr),
		index_buffer_(nullptr),
		vertex_declaration_(nullptr)
	{ }

	Mesh::~Mesh()
	{
		if (!context)
		{
			System::UserOutput::Display("Direct3D context has been destroyed before mesh.");
		}
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

	Mesh* Mesh::CreateRightHanded(Context *&i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count)
	{
		if (i_index_count % 3 != 0)		//index buffer must be a list of triangles
		{
			System::UserOutput::Display("Cannot create a Mesh with non-triangular data. (Ensure number of indices is divisible by 3)");
			return nullptr;
		}
		SwapIndexOrder(i_indices, i_index_count);
		Mesh *mesh = CreateLeftHanded(i_context, i_vertices, i_vertex_count, i_indices, i_index_count);
		SwapIndexOrder(i_indices, i_index_count);
		return mesh;
	}

	//Create a mesh with LEFT-HANDED indices
	Mesh* Mesh::CreateLeftHanded(Context *&i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count)
	{
		if (i_index_count % 3 != 0)		//index buffer must be a list of triangles
		{
			System::UserOutput::Display("Cannot create a Mesh with non-triangular data. (Ensure number of indices is divisible by 3)");
			return nullptr;
		}

		Mesh *mesh = new Mesh(i_vertex_count, i_index_count, i_context);
		if (!mesh)
		{
			System::UserOutput::Display("Failed to create Mesh, due to insufficient memory.", "Mesh Loading Error");
			return nullptr;
		}

		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = GetVertexProcessingUsage(i_context->get_direct3dDevice(), usage);
			if (FAILED(result))
			{
				System::UserOutput::Display("Unable to get vertex processing usage information");
				delete mesh;
				return nullptr;
			}
			// Our code will only ever write to the buffer
			usage |= D3DUSAGE_WRITEONLY;
		}

		//Create the Vertex Buffer
		{
			// Initialize the vertex format
			{
				// These elements must match the VertexFormat::sVertex layout struct exactly.
				// They instruct Direct3D how to match the binary data in the vertex buffer
				// to the input elements in a vertex shader
				// (by using D3DDECLUSAGE enums here and semantics in the shader,
				// so that, for example, D3DDECLUSAGE_POSITION here matches with POSITION in shader code).
				// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
				D3DVERTEXELEMENT9 vertexElements[] =
				{
					// Stream 0

					// POSITION
					// 2 floats == 8 bytes
					// Offset = 0
					{ 0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

					// COLOR0
					// D3DCOLOR == 4 bytes
					// Offset = 8
					{ 0, 8, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

					// The following marker signals the end of the vertex declaration
					D3DDECL_END()
				};
				HRESULT result = i_context->get_direct3dDevice()->CreateVertexDeclaration(vertexElements, &mesh->vertex_declaration_);
				if (SUCCEEDED(result))
				{
					result = i_context->get_direct3dDevice()->SetVertexDeclaration(mesh->vertex_declaration_);
					if (FAILED(result))
					{
						System::UserOutput::Display("Direct3D failed to set the vertex declaration");
						delete mesh;
						return nullptr;
					}
				}
				else
				{
					System::UserOutput::Display("Direct3D failed to create a Direct3D9 vertex declaration");
					delete mesh;
					return nullptr;
				}
			}

			// Create a vertex buffer
			{
				const unsigned int bufferSize = static_cast<unsigned int>(i_vertex_count * sizeof(Vertex));
				// We will define our own vertex format
				const DWORD useSeparateVertexDeclaration = 0;
				// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
				const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
				HANDLE* const notUsed = NULL;
				const HRESULT result = i_context->get_direct3dDevice()->CreateVertexBuffer(static_cast<UINT>(bufferSize), usage, useSeparateVertexDeclaration, useDefaultPool,
					&mesh->vertex_buffer_, notUsed);
				if (FAILED(result))
				{
					System::UserOutput::Display("Direct3D failed to create a vertex buffer");
					delete mesh;
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
					const HRESULT result = mesh->vertex_buffer_->Lock(lockEntireBuffer, lockEntireBuffer,
						reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
					if (FAILED(result))
					{
						System::UserOutput::Display("Direct3D failed to lock the vertex buffer");
						delete mesh;
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
					const HRESULT result = mesh->vertex_buffer_->Unlock();
					if (FAILED(result))
					{
						System::UserOutput::Display("Direct3D failed to unlock the vertex buffer");
						delete mesh;
						return nullptr;
					}
				}
			}
		}

		//Create the Index Buffer
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
			{
				// Before the index buffer can be changed it must be "locked"
				uint32_t *indexData;
				{
					const unsigned int lockEntireBuffer = 0;
					const DWORD useDefaultLockingBehavior = 0;
					const HRESULT result = mesh->index_buffer_->Lock(lockEntireBuffer, lockEntireBuffer,
						reinterpret_cast<void**>(&indexData), useDefaultLockingBehavior);
					if (FAILED(result))
					{
						System::UserOutput::Display("Direct3D failed to lock the index buffer");
						delete mesh;
						return nullptr;
					}
				}
				// Fill the buffer
				{
					memcpy(indexData, i_indices, i_index_count * sizeof(*i_indices));
					
					//Preferred, but VC++ doesn't like unchecked iterators
					//std::copy(i_indices, i_indices + i_index_count, indexData);
				}
				// The buffer must be "unlocked" before it can be used
				{
					const HRESULT result = mesh->index_buffer_->Unlock();
					if (FAILED(result))
					{
						System::UserOutput::Display("Direct3D failed to unlock the index buffer");
						delete mesh;
						return nullptr;
					}
				}
			}
		}

		return mesh;
	}

	bool Mesh::Draw()
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
			// We are using triangles as the "primitive" type,
			// and we have defined the vertex buffer as a triangle list
			// (meaning that every triangle is defined by three vertices)
			const D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;
			// It's possible to start rendering primitives in the middle of the stream
			const UINT indexOfFirstVertexToRender = 0;
			const UINT indexOfFirstIndexToUse = 0;
			const UINT primitiveCountToRender = static_cast<UINT>(index_count_ / 3);	// How many triangles will be drawn?
			result = context->get_direct3dDevice()->DrawIndexedPrimitive(primitiveType,
				indexOfFirstVertexToRender, indexOfFirstVertexToRender, 4,
				indexOfFirstIndexToUse, 2);
			return SUCCEEDED(result);
		}
	}
}
