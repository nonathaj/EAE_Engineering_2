#ifndef _ENGINE_SYSTEM_FILELOADER_H
#define _ENGINE_SYSTEM_FILELOADER_H

#include <string>

namespace Lame
{
	namespace File
	{
		//Loads a binary file into a temporary buffer, and returns a pointer to that buffer
		char* LoadBinary(const std::string& i_file_name, size_t* o_fileLength = nullptr);

		//Loads a binary mesh file and separates the data out (buffer must be manually deleted after call, to dispose of data in buffer)
		template<typename CountType, typename VertexType, typename IndexType>
		char* LoadMeshData(const std::string& i_mesh_binary_file, CountType& o_vertex_count, CountType& o_index_count, VertexType*& o_vertices, IndexType*& o_indices, size_t* o_file_length = nullptr);
	}

	namespace File
	{
		template<typename CountType, typename VertexType, typename IndexType>
		char* LoadMeshData(const std::string& i_mesh_binary_file, CountType& o_vertex_count, CountType& o_index_count, VertexType*& o_vertices, IndexType*& o_indices, size_t* o_file_length)
		{
			size_t fileLength;
			char *fileData = Lame::File::LoadBinary(i_mesh_binary_file, &fileLength);
			if (!fileData)
				return nullptr;

			//find the actual location of our data
			CountType *vertex_count = reinterpret_cast<CountType*>(fileData);
			CountType *index_count = vertex_count + 1;
			o_vertices = reinterpret_cast<VertexType*>(index_count + 1);
			o_indices = reinterpret_cast<IndexType*>(o_vertices + *vertex_count);

			//if the end of indices is beyond the end of the file
			if (reinterpret_cast<void*>(o_indices + *index_count) > fileData + fileLength)
			{
				delete[] fileData;
				return nullptr;
			}

			o_vertex_count = *vertex_count;
			o_index_count = *index_count;

			if (o_file_length)
				*o_file_length = fileLength;
			return fileData;
		}
	}
}

#endif //_ENGINE_SYSTEM_FILELOADER_H
