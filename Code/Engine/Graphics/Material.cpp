
#include <fstream>
#include <sstream>

#include "Material.h"
#include "../System/FileLoader.h"
#include "../System/UserOutput.h"

namespace Lame
{
	Material* Material::Create(std::shared_ptr<Lame::Context> i_context, std::string i_path)
	{
		size_t fileLength;
		char *fileData = System::File::LoadBinary(i_path, &fileLength);
		if (!fileData)
			return nullptr;

		//load the effect
		uint8_t *effectStringLength = reinterpret_cast<uint8_t*>(fileData);
		char *effectLocation = reinterpret_cast<char*>(effectStringLength + 1);
		std::shared_ptr<Effect> effect = std::shared_ptr<Effect>(Effect::Create(i_context, effectLocation));
		if (!effect)
		{
			delete[] fileData;
			return nullptr;
		}

		//setup the parameters
		uint8_t *parameterCount = reinterpret_cast<uint8_t*>(effectLocation + *effectStringLength + 1);
		Material::Parameter *params = nullptr;
		uint8_t *currentParamNameLen = nullptr;
		char *currentParamName = nullptr;
		if (*parameterCount > 0)
		{
			params = reinterpret_cast<Material::Parameter*>(parameterCount + 1);
			currentParamNameLen = reinterpret_cast<uint8_t*>(params + *parameterCount);
			currentParamName = reinterpret_cast<char*>(currentParamNameLen + 1);

			//cache each parameter
			for (size_t x = 0; x < *parameterCount; x++)
			{
				if (!effect->CacheConstant(params[x].shader_type, currentParamName, params[x].handle))
				{
					std::stringstream error;
					error << "Failed to cache uniform constant handle \"" << currentParamName << "\" in material "
						<< i_path;
					System::UserOutput::Display(error.str(), "Material loading error");
					delete[] fileData;
					return nullptr;
				}

				currentParamNameLen = reinterpret_cast<uint8_t*>(currentParamName + *currentParamNameLen + 1);
				currentParamName = reinterpret_cast<char*>(currentParamNameLen + 1);
			}
		}

		//if our data extends beyond the size of the file, fail
		if (reinterpret_cast<void*>(parameterCount) > fileData + fileLength || 
			reinterpret_cast<void*>(currentParamNameLen) > fileData + fileLength)
		{
			std::stringstream error;
			error << "Loaded data for material " << i_path << " is invalid";
			System::UserOutput::Display(error.str(), "Material loading error");
			delete[] fileData;
			return nullptr;
		}

		Material *material = new Material(effect);
		if (!material)
		{
			System::UserOutput::Display("Insufficient memory when creating Material", "Material creation error");
			delete[] fileData;
			return nullptr;
		}

		material->parameters_.assign(params, params + *parameterCount);

		delete[] fileData;
		return material;
	}

	bool Material::Bind() const
	{
		bool success = effect()->Bind();

		for (size_t x = 0; x < parameters_.size(); x++)
		{
			success = success && effect()->SetConstant(
				parameters_[x].shader_type, 
				parameters_[x].handle, 
				parameters_[x].value, 
				parameters_[x].valueCount
				);
		}

		return success;
	}
}
