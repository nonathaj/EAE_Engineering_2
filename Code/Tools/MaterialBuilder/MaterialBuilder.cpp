
#include "MaterialBuilder.h"

#include <sstream>
#include <cassert>
#include <fstream>
#include <locale>
#include <cstdint>
#include <map>
#include <vector>

#include "../BuilderHelper/UtilityFunctions.h"
#include "../../Engine/Windows/Functions.h"
#include "../../External/Lua/Includes.h"

#include "../../Engine/Graphics/Material.h"
#include "../../External/Lua/LuaHelper.h"

namespace
{
	void ToLower(std::string &io_str);
}

bool MaterialBuilder::Build(const std::vector<std::string>&)
{
	////////////////////////////////////////////
	//Data we need
	////////////////////////////////////////////
	std::string effectLocation;
	std::vector<std::string> uniform_names;
	std::vector<Lame::Material::Parameter> uniforms;
	std::vector<std::string> uniform_texture_names;

	////////////////////////////////////////////
	//Load data from Lua
	////////////////////////////////////////////
	{
		LuaHelper::LuaStack *stack = LuaHelper::LuaStack::Create(m_path_source);
		if (!stack)
		{
			eae6320::OutputErrorMessage("Failed to open the file and create lua state.", m_path_source);
			return false;
		}

		//effect file location
		stack->Push("effect");
		if (!stack->SwapTableKey() || !stack->Peek(effectLocation))
		{
			eae6320::OutputErrorMessage("Invalid effect value.", m_path_source);
			delete[] stack;
			return false;
		}
		stack->Pop();

		stack->Push("uniforms");
		if (stack->SwapTableKey() && stack->IsTable())
		{
			size_t uniformCount = stack->TableLength();
			for (size_t x = 0; x < uniformCount; x++)
			{
				//individual uniform
				Lame::Material::Parameter param;
				stack->Push(static_cast<lua_Unsigned>(x + 1));
				if (stack->SwapTableKey() && stack->IsTable())
				{
					//name and shader type
					const std::string name = "name";
					const std::string shader = "shader";
					std::map<std::string, std::string> strs;
					if (stack->PeekDictionary(strs))
					{
						if(strs.find(name) != strs.end())
							uniform_names.push_back(strs[name]);
						else
						{
							std::stringstream error;
							error << "Uniform " << x << " is missing the \"name\" attribute";
							eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
							delete stack;
							return false;
						}

						if (strs.find(shader) != strs.end())
						{
							ToLower(strs[shader]);
							if (strs[shader] == "fragment")
								param.shader_type = Lame::Effect::Shader::Fragment;
							else if (strs[shader] == "vertex")
								param.shader_type = Lame::Effect::Shader::Vertex;
						}
						else
						{
							std::stringstream error;
							error << "Uniform " << x << " is missing the \"shader\" attribute";
							eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
							delete stack;
							return false;
						}
					}
					else
					{
						std::stringstream error;
						error << "Invalid uniform data table for uniform " << x;
						eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
						delete stack;
						return false;
					}

					//actual parameter data
					const char* floatValueKey = "value";
					const char* textureKey = "texture";
					bool hasFloats = stack->HasValue(floatValueKey);
					bool hasTexture = strs.find(textureKey) != strs.end();

					if (hasFloats && hasTexture)
					{
						//ERROR we shouldn't have both
						std::stringstream error;
						error << "Uniform " << x << " contains both \"value\" and \"texture\" parameter values.  You may only use one of these for each parameter.";
						eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
						delete stack;
						return false;
					}
					else if (!hasFloats && !hasTexture)
					{
						//ERROR we have to have one of them
						std::stringstream error;
						error << "Uniform " << x << " does not contain a \"value\" or \"texture\" parameter value";
						eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
						delete stack;
						return false;
					}
					else if (hasFloats)
					{
						std::vector<double> values;
						stack->Push(floatValueKey);
						if (stack->SwapTableKey() && stack->PeekArray(values) && values.size() > 0 && values.size() <= 4)
						{
							for (size_t x = 0; x < values.size(); x++)
								param.value[x] = static_cast<float>(values[x]);
							param.valueCount = static_cast<uint8_t>(values.size());
						}
						else
						{
							std::stringstream error;
							error << "Invalid uniform float value data for uniform " << x;
							eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
							delete stack;
							return false;
						}
						stack->Pop();
					}
					else if(hasTexture)
						uniform_texture_names.push_back(strs[textureKey]);

					//we need to ensure that the number of texture names matches the number of uniforms
					if (!hasTexture)
						uniform_texture_names.push_back("");
				}
				else
				{
					std::stringstream error;
					error << "Invalid uniform data table for uniform " << x;
					eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
					delete stack;
					return false;
				}
				stack->Pop();

				uniforms.push_back(param);
			}
		}
		else
		{
			eae6320::OutputErrorMessage("Invalid uniforms table", m_path_source);
			delete[] stack;
			return false;
		}
		stack->Pop();
	}

	////////////////////////////////////////////
	//Fix up loaded data
	////////////////////////////////////////////
	{
		if (uniforms.size() != uniform_names.size() || uniforms.size() != uniform_texture_names.size())
		{
			eae6320::OutputErrorMessage("Error parsing uniform data.", m_path_source);
			return false;
		}

		//Add the relative folder location of the built assets for the effect and texture locations
		std::string relativeFolder, outError;
		if (!eae6320::GetEnvironmentVariableA("GameDataDir", relativeFolder, &outError))
		{
			std::stringstream error;
			error << "Failed to load GameDataDir environment variable. " << outError;
			eae6320::OutputErrorMessage(error.str().c_str());
			return false;
		}
		effectLocation = relativeFolder + effectLocation;

#if EAE6320_PLATFORM_D3D
		const size_t maxHandleStringLength = std::numeric_limits<uintptr_t>::max();
#elif EAE6320_PLATFORM_GL
		const size_t maxHandleStringLength = static_cast<size_t>(std::numeric_limits<Lame::Effect::ConstantHandle>::max());
#else
#error Platform must define a max length for the ConstantHandle
#endif
		//update the handles to contain the length of the parameter's name
		for (size_t x = 0; x < uniforms.size(); x++)
		{
			//append the relative built assets folder
			uniform_texture_names[x] = relativeFolder + uniform_texture_names[x];

			//fill the texture slot with the texture name length
			uniforms[x].texture = reinterpret_cast<Lame::Texture*>(static_cast<uintptr_t>(uniform_texture_names[x].size()));

			//fill the handle slot with the parameter name length
#if EAE6320_PLATFORM_D3D
			uniforms[x].handle = std::make_tuple(reinterpret_cast<char*>(static_cast<uintptr_t>(uniform_names[x].size())), static_cast<DWORD>(0));
#elif EAE6320_PLATFORM_GL
			uniforms[x].handle = static_cast<Lame::Effect::ConstantHandle>(uniform_names[x].size());
#else
#error Platform undefined for setting the uniform handle size
#endif
			//warn about parameter name string length being too long
			if (uniform_names[x].size() > maxHandleStringLength)
			{
				std::stringstream error;
				error << "Length of parameter name (" << uniform_names[x] << ") for material "
					<< m_path_source << " is greater than " << maxHandleStringLength 
					<< " (actual value=" << uniform_names[x].size() << ")";
				eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
				return false;
			}
		}
	}

	////////////////////////////////////////////
	//Write data to binary
	////////////////////////////////////////////
	{
		std::ofstream out(m_path_target, std::ofstream::binary);
		if (!out)
		{
			eae6320::OutputErrorMessage("Failed to open the output file for writing", m_path_source);
			return false;
		}

		//effect length and string
		uint8_t effectStringLength = static_cast<uint8_t>(effectLocation.size());
		//warn about effect string length being too long
		if (effectLocation.length() > std::numeric_limits<uint8_t>::max())
		{
			std::stringstream error;
			error << "Length of effect path for material " << m_path_source << " is greater than "
				<< sizeof(effectStringLength) << " (actual value=" << effectLocation.length() << ")";
			eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
			return false;
		}

		out.write(reinterpret_cast<char*>(&effectStringLength), sizeof(effectStringLength));
		out.write(effectLocation.c_str(), effectLocation.size() + 1);

		//warn about parameter count being too big
		if (uniforms.size() > std::numeric_limits<uint8_t>::max())
		{
			std::stringstream error;
			error << "Number of parameters for material " << m_path_source << " is greater than "
				<< std::numeric_limits<uint8_t>::max() << " (actual value=" << uniforms.size() << ")";
			eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
			return false;
		}
		uint8_t paramCount = static_cast<uint8_t>(uniforms.size());
		out.write(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));

		if (uniforms.size() > 0)
		{
			//parameter values
			out.write(reinterpret_cast<char*>(uniforms.data()), sizeof(uniforms.data()[0]) * uniforms.size());

			//parameter names and texture names
			for (size_t x = 0; x < uniforms.size(); x++)
			{
				out.write(uniform_names[x].c_str(), uniform_names[x].size() + 1);
				if (uniforms[x].texture)
					out.write(uniform_texture_names[x].c_str(), uniform_texture_names.size() + 1);
			}
		}

		out.close();
	}
	return true;
}

namespace
{
	void ToLower(std::string &io_str)
	{
		for (size_t x = 0; x < io_str.size(); x++)
			io_str[x] = tolower(io_str[x]);
	}
}
