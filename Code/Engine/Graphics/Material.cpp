
#include <fstream>
#include <sstream>

#include "Material.h"
#include "../System/FileLoader.h"
#include "../System/UserOutput.h"
#include "Texture.h"

#include "../System/Console.h"

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
		char *currentParamName = nullptr;
		size_t uniform_name_length = 0;
		if (*parameterCount > 0)
		{
			params = reinterpret_cast<Material::Parameter*>(parameterCount + 1);
			currentParamName = reinterpret_cast<char*>(params + *parameterCount);

			//cache each parameter
			for (size_t x = 0; x < *parameterCount; x++)
			{
#if EAE6320_PLATFORM_D3D
				uniform_name_length = static_cast<size_t>(reinterpret_cast<uintptr_t>(std::get<0>(params[x].handle)));
#elif EAE6320_PLATFORM_GL
				uniform_name_length = static_cast<size_t>(params[x].handle);
#else
#error Platform must define a conversion from params[x].handle to uniform_name_length
#endif
				if (!effect->CacheConstant(params[x].shader_type, currentParamName, params[x].handle))
				{
					std::stringstream error;
					error << "Failed to cache uniform constant handle \"" << currentParamName << "\" in material "
						<< i_path;
					System::UserOutput::Display(error.str(), "Material loading error");
					delete[] fileData;
					return nullptr;
				}

				currentParamName = reinterpret_cast<char*>(currentParamName + uniform_name_length + 1);

				//if we have a texture, then currentParamName now points to the texture name.
				if (params[x].texture)
				{
					uniform_name_length = static_cast<size_t>(reinterpret_cast<uintptr_t>(params[x].texture));

					params[x].texture = Texture::Create(i_context, currentParamName);
					if (!params[x].texture)
					{
						delete[] fileData;
						return nullptr;
					}

					//point at the next parameter name
					currentParamName = reinterpret_cast<char*>(currentParamName + uniform_name_length + 1);
				}
			}
		}

		//if our data extends beyond the size of the file, fail
		if (reinterpret_cast<void*>(parameterCount) > fileData + fileLength || 
			reinterpret_cast<void*>(currentParamName) > fileData + fileLength)
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

	Material::~Material()
	{
		for (size_t x = 0; x < parameters_.size(); x++)
		{
			if (parameters_[x].texture != nullptr)
			{
				delete parameters_[x].texture;
				parameters_[x].texture = nullptr;
			}
		}
	}

	bool Material::Bind() const
	{
		bool success = effect()->Bind();

		for (size_t x = 0; x < parameters_.size(); x++)
		{
			if (parameters_[x].texture != nullptr)
			{
				success = success && effect()->SetConstant(parameters_[x].shader_type,
					parameters_[x].handle, parameters_[x].texture
#if defined( LAME_EFFECT_TEXTURE_INDEX_REQUIRED_TO_SET_TEXTURE )
					, x
#endif
					);
			}
			else
			{
				success = success && effect()->SetConstant(parameters_[x].shader_type,
					parameters_[x].handle, parameters_[x].value, parameters_[x].valueCount);
			}
		}

		return success;
	}


	bool Material::AddParameter(const std::string& i_param_name, const Effect::Shader i_shader_type, Texture* i_texture)
	{
		if (!i_texture)
			return false;

		Parameter p;
		p.texture = i_texture;
		p.shader_type = i_shader_type;

		return AddParameter(i_param_name, p);
	}

	bool Material::AddParameter(const std::string& i_param_name, const Effect::Shader i_shader_type, const std::string& i_texture_path)
	{
		return AddParameter(i_param_name, i_shader_type, Lame::Texture::Create(effect()->get_context(), i_texture_path));
	}
	
	bool Material::AddParameter(const std::string& i_param_name, const Effect::Shader i_shader_type, const float* i_vals, const size_t i_vals_count)
	{
		if (!i_vals || i_vals_count > 4 || i_vals_count <= 0)
			return false;

		Parameter p;
		for (size_t x = 0; x < i_vals_count; x++)
			p.value[x] = i_vals[x];
		p.valueCount = static_cast<uint8_t>(i_vals_count);
		p.shader_type = i_shader_type;
		p.texture = nullptr;

		return AddParameter(i_param_name, p);
	}

	bool Material::AddParameter(const std::string& i_param_name, const Effect::Shader i_shader_type, const float i_val)
	{
		return AddParameter(i_param_name, i_shader_type, &i_val, 1);
	}

	bool Material::AddParameter(const std::string& i_param_name, Parameter& i_param)
	{
		if (!effect()->CacheConstant(i_param.shader_type, i_param_name, i_param.handle))
		{
			std::stringstream error;
			error << "Failed to cache uniform constant handle \"" << i_param_name << "\"";
			System::UserOutput::Display(error.str());
			return false;
		}

		for (size_t x = 0; x < parameters_.size(); x++)
		{
			if (parameters_[x].handle == i_param.handle)
			{
				std::stringstream error;
				error << "Material already contains a definition for uniform constant handle \"" << i_param_name << "\"";
				System::UserOutput::Display(error.str());
				return false;
			}
		}

		parameters_.push_back(i_param);
		return true;
	}
}
