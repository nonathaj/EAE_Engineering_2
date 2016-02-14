
#include "FileLoader.h"

#include <fstream>
#include <sstream>

#include "UserOutput.h"

namespace Lame
{
	namespace File
	{
		char* LoadBinary(const std::string& i_file_name, size_t* o_fileLength)
		{
			//open the file
			std::ifstream in(i_file_name, std::ifstream::binary);
			if (!in)
			{
				std::stringstream error;
				error << "Failed to open " << i_file_name << " binary file";
				Lame::UserOutput::Display(error.str());
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
				std::stringstream error;
				error << "Failed to create temporary buffer for " << i_file_name;
				Lame::UserOutput::Display(error.str());
				return nullptr;
			}

			//read the data from the file
			in.read(fileData, fileLength);

			//close the file
			in.close();

			if (o_fileLength)
				*o_fileLength = fileLength;

			return fileData;
		}
	}
}
