
#include "Effect.h"

#include <fstream>
#include <sstream>

#include "../System/UserOutput.h"
#include "../System/FileLoader.h"
#include "Color.h"

namespace Lame
{
	bool Effect::SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const Lame::Vector3 &i_val)
	{
		return SetConstant(i_shader, i_constant, reinterpret_cast<const float*>(&i_val), 3);
	}

	bool Effect::SetConstant(const Shader &i_shader, const ConstantHandle &i_constant, const Color& i_val)
	{
		return SetConstant(i_shader, i_constant, reinterpret_cast<const float*>(&i_val), 4);
	}

	Effect* Effect::Create(std::shared_ptr<Context> i_context, const std::string& i_effect_path)
	{
		size_t fileLength;
		char *fileData = Lame::File::LoadBinary(i_effect_path, &fileLength);
		if (!fileData)
			return nullptr;

		//find the actual location of our data
		Lame::EnumMask<RenderState> renderMask = *reinterpret_cast<Lame::EnumMask<RenderState>*>(fileData);
		uint8_t vertexStringLength = *reinterpret_cast<uint8_t*>(fileData + sizeof(renderMask));
		const char* vertex = reinterpret_cast<const char *>(fileData + sizeof(renderMask) + sizeof(vertexStringLength));
		const char* fragment = reinterpret_cast<const char *>(vertex + vertexStringLength + 1);

		if (fragment >= fileData + fileLength)
		{
			std::stringstream error;
			error << "Loaded data for " << i_effect_path << " is invalid";
			Lame::UserOutput::Display(error.str(), "Effect loading error");
			delete[] fileData;
			return nullptr;
		}

		//create the effect and cleanup the temporary buffer
		Effect *effect = Create(i_context, vertex, fragment, renderMask);
		delete[] fileData;
		return effect;
	}
}
