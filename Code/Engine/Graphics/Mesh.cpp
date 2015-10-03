
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <functional>
#include <string>

#include "Mesh.h"
#include "../System/Console.h"

#include "Vertex.h"

namespace Lame
{
	Mesh* Mesh::Create(const Context *i_context, std::string i_mesh_path)
	{
		//Open the file
		std::ifstream in(i_mesh_path, std::ifstream::binary);
		if (!in)
		{
			DEBUG_PRINT("Failed to open %s mesh file.", i_mesh_path.c_str());
			return nullptr;
		}

		//find the file length
		in.seekg(0, in.end);
		size_t fileLength = static_cast<size_t>(in.tellg());
		in.seekg(0, in.beg);
		
		//Create a buffer for the data
		char *fileData = new char[fileLength];
		if (!fileData)
		{
			DEBUG_PRINT("Failed to create temporary buffer for mesh");
			return nullptr;
		}

		//read the data from the file
		in.read(fileData, fileLength);

		//close the file
		in.close();

		//find the actual location of our data
		uint32_t *vertex_count = reinterpret_cast<uint32_t*>(fileData);
		uint32_t *index_count = vertex_count + 1;
		Vertex *vertices = reinterpret_cast<Vertex*>(index_count + 1);
		uint32_t *indices = reinterpret_cast<uint32_t*>(vertices + *vertex_count);
		
		//if the end of indices is beyond the end of the file
		if (reinterpret_cast<void*>(indices + *index_count) > fileData + fileLength)
		{
			DEBUG_PRINT("Loaded data for mesh %s is invalid", i_mesh_path.c_str());
			delete[] fileData;
			return nullptr;
		}

		//create the mesh
		Mesh *mesh = Create(i_context, vertices, *vertex_count, indices, *index_count);

		//cleanup the loaded file
		delete[] fileData;

		return mesh;
	}
}
