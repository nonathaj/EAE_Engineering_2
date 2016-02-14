
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <functional>
#include <string>

#include "RenderableMesh.h"
#include "../System/UserOutput.h"
#include "../System/Console.h"
#include "../System/FileLoader.h"

#include "../Core/Vector2.h"
#include "../Core/Vector3.h"

#include "../Core/Vertex.h"
#include "../Core/Mesh.h"

namespace Lame
{
	void RenderableMesh::SwapIndexOrder(uint32_t *i_indices, size_t i_index_count)
	{
		for (size_t x = 0; x < i_index_count; x += 3)
			std::swap(i_indices[x], i_indices[x + 2]);
	}

	RenderableMesh* RenderableMesh::Create(const bool i_static, std::shared_ptr<Context> i_context, const std::string& i_mesh_path)
	{
		uint32_t vertex_count;
		uint32_t index_count;
		Vertex *vertices;
		uint32_t *indices;
		char *fileData = File::LoadMeshData(i_mesh_path, vertex_count, index_count, vertices, indices);
		if (!fileData)
			return nullptr;

		//create the mesh
		RenderableMesh *mesh = nullptr;
#if EAE6320_PLATFORM_D3D
		mesh = CreateLeftHandedTriList(i_static, i_context, vertices, vertex_count, indices, index_count);
#elif EAE6320_PLATFORM_GL
		mesh = CreateRightHandedTriList(i_static, i_context, vertices, *vertex_count, indices, *index_count);
#else
#error No Creation function for renderable meshes loaded from file
#endif

		//cleanup the loaded file
		delete[] fileData;

		return mesh;
	}

	RenderableMesh* RenderableMesh::Create(const bool i_static, std::shared_ptr<Context> i_context, const Mesh& i_mesh)
	{
		RenderableMesh* rm = CreateEmpty(i_static, i_context, i_mesh.primitive_type(), i_mesh.vertices_RO().size(), i_mesh.indices_RO().size());
		if (!rm)
			return nullptr;
		if (!rm->UpdateVertices(i_mesh.vertices_RO().data()) || !rm->UpdateIndices(i_mesh.indices_RO().data()))
		{
			delete rm;
			return nullptr;
		}
		return rm;
	}

	size_t RenderableMesh::primitive_count() const
	{
		return Lame::Mesh::GetPrimitiveCount(primitive_type(), index_count_ > 0 ? index_count_ : vertex_count_);
	}
}
