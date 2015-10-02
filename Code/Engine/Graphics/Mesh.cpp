
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
	Mesh* Mesh::Create(std::string i_mesh_path)
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
		size_t fileLength = in.tellg();
		in.seekg(0, in.beg);
		
		//read the data from the file
		char *fileData = new char[fileLength];
		in.read(fileData, fileLength);

		//close the file
		in.close();

		//find the actual location of our data
		uint32_t *vertex_count = reinterpret_cast<uint32_t*>(fileData);
		uint32_t *index_count = vertex_count + 1;
		Vertex *vertices = reinterpret_cast<Vertex*>( fileData + 2 * sizeof(uint32_t) );
		uint32_t *indices = reinterpret_cast<uint32_t*>( fileData + 2 * sizeof(uint32_t) + *vertex_count * sizeof(Vertex));

		//create the mesh
		Mesh *mesh = Create(vertices, *vertex_count, indices, *index_count);

		//cleanup the loaded file
		delete[] fileData;

		return mesh;
	}
}
