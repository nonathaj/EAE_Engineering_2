#ifndef _ENGINE_SYSTEM_FILELOADER_H
#define _ENGINE_SYSTEM_FILELOADER_H

#include <string>

namespace Lame
{
	namespace File
	{
		//Loads a binary file into a temporary buffer, and returns a pointer to that buffer
		char* LoadBinary(std::string i_file_name, size_t* o_fileLength = nullptr);
	}
}

#endif //_ENGINE_SYSTEM_FILELOADER_H
