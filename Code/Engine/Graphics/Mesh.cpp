
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <functional>
#include <string>

#include "Mesh.h"
#include "../System/UserOutput.h"
#include "../System/Console.h"
#include "../System/FileLoader.h"

#include "Vertex.h"

namespace Lame
{
	void Mesh::SwapIndexOrder(uint32_t *i_indices, size_t i_index_count)
	{
		for (size_t x = 0; x < i_index_count; x += 3)
			std::swap(i_indices[x], i_indices[x + 2]);
	}

	Mesh* Mesh::Create(std::shared_ptr<Context> i_context, std::string i_mesh_path)
	{
		std::string errorHeader = "Mesh loading error";

		size_t fileLength;
		char *fileData = System::File::LoadBinary(i_mesh_path, &fileLength);

		//find the actual location of our data
		uint32_t *vertex_count = reinterpret_cast<uint32_t*>(fileData);
		uint32_t *index_count = vertex_count + 1;
		Vertex *vertices = reinterpret_cast<Vertex*>(index_count + 1);
		uint32_t *indices = reinterpret_cast<uint32_t*>(vertices + *vertex_count);
		
		//if the end of indices is beyond the end of the file
		if (reinterpret_cast<void*>(indices + *index_count) > fileData + fileLength)
		{
			std::stringstream error;
			error << "Loaded data for mesh " << i_mesh_path << " is invalid";
			System::UserOutput::Display(error.str(), errorHeader);
			delete[] fileData;
			return nullptr;
		}

		//create the mesh
		Mesh *mesh = nullptr;
#if EAE6320_PLATFORM_D3D
		mesh = CreateLeftHanded(i_context, vertices, *vertex_count, indices, *index_count);
#elif EAE6320_PLATFORM_GL
		mesh = CreateRightHanded(i_context, vertices, *vertex_count, indices, *index_count);
#endif

		//cleanup the loaded file
		delete[] fileData;

		return mesh;
	}
}
