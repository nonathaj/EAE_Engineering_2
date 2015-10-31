
#include "Effect.h"

#include <fstream>
#include <sstream>

#include "../System/UserOutput.h"
#include "../System/FileLoader.h"

namespace Lame
{
	char const * const Effect::PositionUniformName = "position_offset";
	const Engine::HashedString Effect::PositionUniformId(Effect::PositionUniformName);

	bool Effect::SetPosition(eae6320::Math::cVector i_position)
	{
		return SetConstant(PositionUniformId, i_position);
	}

	Effect* Effect::Create(std::shared_ptr<Context> i_context, const std::string& i_effect_path)
	{
		size_t fileLength;
		char *fileData = System::File::LoadBinary(i_effect_path, &fileLength);

		//find the actual location of our data
		uint8_t vertexStringLength = *reinterpret_cast<uint8_t*>(fileData);
		const char* vertex = reinterpret_cast<const char *>(fileData + sizeof(vertexStringLength));
		const char* fragment = reinterpret_cast<const char *>(vertex + vertexStringLength + 1);

		if (fragment >= fileData + fileLength)
		{
			std::stringstream error;
			error << "Loaded data for " << i_effect_path << " is invalid";
			System::UserOutput::Display(error.str(), "Effect loading error");
			delete[] fileData;
			return nullptr;
		}

		//create the effect and cleanup the temporary buffer
		Effect *effect = Create(i_context, vertex, fragment);
		delete[] fileData;
		return effect;
	}
}
