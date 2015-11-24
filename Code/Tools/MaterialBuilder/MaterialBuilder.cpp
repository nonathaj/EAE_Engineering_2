
#include "MaterialBuilder.h"

#include <sstream>
#include <cassert>
#include <fstream>
#include <locale>
#include <cstdint>

#include "../BuilderHelper/UtilityFunctions.h"
#include "../../Engine/Windows/Functions.h"
#include "../../External/Lua/Includes.h"

#include "../../Engine/Graphics/Material.h"

namespace
{
	lua_State* LoadAssetTable(const std::string &i_path);
	bool LoadFromTable(lua_State* io_luaStateFrom, const std::string& i_key, std::string& o_value);
	bool LoadFromTable(lua_State* io_luaStateFrom, const std::string& i_key, bool& o_value);
	bool LoadFromTable(lua_State* io_luaStateFrom, Lame::Material::Parameter& o_value, std::string &o_name);
	void ToLower(std::string &io_str);
	double GetDoubleFromTable(lua_State* io_luaStateFrom, size_t i_indexInTable);
}

bool MaterialBuilder::Build(const std::vector<std::string>&)
{
	lua_State *luaState = LoadAssetTable(m_path_source);
	if (!luaState)
	{
		eae6320::OutputErrorMessage("Failed to open the input file for reading", m_path_source);
		return false;
	}

	/////////////////////////////////////////
	//read out the lua data

	//effect location
	std::string effectLocation;
	if (!LoadFromTable(luaState, "effect", effectLocation))
	{
		eae6320::OutputErrorMessage("\"effect\" value missing, or not a string in material", m_path_source);
		lua_close(luaState);
		return false;
	}

	//uniform table
	lua_pushstring(luaState, "uniforms");
	lua_gettable(luaState, -2);
	if (!lua_istable(luaState, -1))
	{
		eae6320::OutputErrorMessage("\"uniforms\" is not a table in material", m_path_source);
		lua_close(luaState);
		return false;
	}
		
	//uniform values
	size_t parameterCount = static_cast<size_t>(luaL_len(luaState, -1));
	std::vector<std::string> uniform_names(parameterCount);
	std::vector<Lame::Material::Parameter> uniforms(parameterCount);
	for (size_t x = 0; x < parameterCount; x++)
	{
		lua_pushinteger(luaState, x + 1);
		lua_gettable(luaState, -2);

		//load the parameter data
		std::string name;
		Lame::Material::Parameter uniform;
		if (lua_istable(luaState, -1))
		{
			if (!LoadFromTable(luaState, uniform, name))
			{
				std::stringstream error;
				error << "Invalid value for uniform parameter " << name << " in material";
				eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
				lua_close(luaState);
				return false;
			}
		}
		else
		{
			std::stringstream error;
			error << "uniform parameter " << x << " is not a table";
			eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
			lua_close(luaState);
			return false;
		}
			
		uniform_names.push_back(name);
		uniforms.push_back(uniform);

		lua_pop(luaState, 1);
	}

	//pop the uniform table
	lua_pop(luaState, 1);
	//close the lua table
	lua_close(luaState);

	/////////////////////////////////////////
	//write the data

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

	//number of parameters
	uint8_t paramCount = static_cast<uint8_t>(parameterCount);

	//warn about parameter count being too big
	if (parameterCount > std::numeric_limits<uint8_t>::max())
	{
		std::stringstream error;
		error << "Number of parameters for material " << m_path_source << " is greater than "
			<< std::numeric_limits<uint8_t>::max() << " (actual value=" << parameterCount << ")";
		eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
		return false;
	}
	out.write(reinterpret_cast<char*>(&paramCount), sizeof(paramCount));

	if (parameterCount > 0)
	{
		//parameter values
		out.write(reinterpret_cast<char*>(uniforms.data()), sizeof(uniforms.data()[0]) * parameterCount);

		//parameter names
		for (size_t x = 0; x < parameterCount; x++)
		{
			uint8_t strSize = static_cast<uint8_t>(uniform_names[x].size());
			//warn about parameter name string length being too long
			if (effectLocation.length() > std::numeric_limits<uint8_t>::max())
			{
				std::stringstream error;
				error << "Length of parameter name (" << uniform_names[x] << ") for material "
					<< m_path_source << " is greater than " << sizeof(strSize) << " (actual value=" 
					<< uniform_names[x].size() << ")";
				eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
				return false;
			}

			out.write(reinterpret_cast<char*>(&strSize), sizeof(strSize));
			out.write(uniform_names[x].c_str(), strSize);
		}
	}

	out.close();
	return true;
}

namespace
{
	void ToLower(std::string &io_str)
	{
		for (size_t x = 0; x < io_str.size(); x++)
			io_str[x] = tolower(io_str[x]);
	}

	double GetDoubleFromTable(lua_State* io_luaStateFrom, size_t i_indexInTable)
	{
		double value = std::numeric_limits<double>::quiet_NaN();

		if (lua_istable(io_luaStateFrom, -1))
		{
			lua_pushinteger(io_luaStateFrom, i_indexInTable);
			lua_gettable(io_luaStateFrom, -2);
			if (lua_isnumber(io_luaStateFrom, -1))
				value = lua_tonumber(io_luaStateFrom, -1);
			lua_pop(io_luaStateFrom, 1);
		}

		return value;
	}

	bool LoadFromTable(lua_State* io_luaStateFrom, Lame::Material::Parameter& o_value, std::string &o_name)
	{
		std::string shader_type;
		if (!LoadFromTable(io_luaStateFrom, "name", o_name) || !LoadFromTable(io_luaStateFrom, "shader", shader_type))
			return false;

		ToLower(shader_type);
		if (shader_type == "fragment")
			o_value.shader_type = Lame::Effect::Shader::Fragment;
		else if (shader_type == "vertex")
			o_value.shader_type = Lame::Effect::Shader::Vertex;
		else
			return false;

		//push values table
		lua_pushstring(io_luaStateFrom, "value");
		lua_gettable(io_luaStateFrom, -2);
		if (!lua_istable(io_luaStateFrom, -1))
			return false;

		o_value.valueCount = static_cast<uint8_t>(luaL_len(io_luaStateFrom, -1));
		for (size_t x = 0; x < o_value.valueCount; x++)
			o_value.value[x] = static_cast<float>(GetDoubleFromTable(io_luaStateFrom, x));

		//pop values table
		lua_pop(io_luaStateFrom, 1);
		return true;
	}

	bool LoadFromTable(lua_State* io_luaStateFrom, const std::string& i_key, std::string& o_value)
	{
		lua_pushstring(io_luaStateFrom, i_key.c_str());
		lua_gettable(io_luaStateFrom, -2);
		if (!lua_isstring(io_luaStateFrom, -1))
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "The value at \"" << i_key << "\" must be a string instead of a " << luaL_typename(io_luaStateFrom, -1);
			eae6320::OutputErrorMessage(decoratedErrorMessage.str().c_str());
			lua_pop(io_luaStateFrom, 1);
			return false;
		}

		o_value = lua_tostring(io_luaStateFrom, -1);
		lua_pop(io_luaStateFrom, 1);
		return true;
	}

	bool LoadFromTable(lua_State* io_luaStateFrom, const std::string& i_key, bool& o_value)
	{
		lua_pushstring(io_luaStateFrom, i_key.c_str());
		lua_gettable(io_luaStateFrom, -2);
		if (!lua_isboolean(io_luaStateFrom, -1))
		{
			std::stringstream decoratedErrorMessage;
			decoratedErrorMessage << "The value at \"" << i_key << "\" must be a boolean instead of a " << luaL_typename(io_luaStateFrom, -1);
			eae6320::OutputErrorMessage(decoratedErrorMessage.str().c_str());
			lua_pop(io_luaStateFrom, 1);
			return false;
		}

		o_value = lua_toboolean(io_luaStateFrom, -1) != 0;
		lua_pop(io_luaStateFrom, 1);
		return true;
	}

	lua_State* LoadAssetTable(const std::string &i_path)
	{
		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State *luaState = luaL_newstate();
		if (!luaState)
		{
			eae6320::OutputErrorMessage("Failed to create a new Lua state\n");
			return false;
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		{
			const int luaResult = luaL_loadfile(luaState, i_path.c_str());
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
				eae6320::OutputErrorMessage(lua_tostring(luaState, -1));
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}
		// Execute the "chunk", which should load the asset
		// into a table at the top of the stack
		{
			const int argumentCount = 0;
			const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
			const int noMessageHandler = 0;
			const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
			if (luaResult == LUA_OK)
			{
				// A well-behaved asset file will only return a single value
				const int returnedValueCount = lua_gettop(luaState);
				if (returnedValueCount == 1)
				{
					// A correct asset file _must_ return a table
					if (!lua_istable(luaState, -1))
					{
						wereThereErrors = true;
						std::stringstream errorStream;
						errorStream << "Asset files must return a table (instead of a " << luaL_typename(luaState, -1) << ")\n";
						eae6320::OutputErrorMessage(errorStream.str().c_str());
						// Pop the returned non-table value
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::stringstream errorStream;
					errorStream << "Asset files must return a single table (instead of " << returnedValueCount << " values)" << "\n";
					eae6320::OutputErrorMessage(errorStream.str().c_str());
					// Pop every value that was returned
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				eae6320::OutputErrorMessage(lua_tostring(luaState, -1));
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}

		// If this comment is reached the asset file was loaded successfully, and its table is now at index -1

	OnExit:

		if (wereThereErrors && luaState)
		{
			// If I haven't made any mistakes
			// there shouldn't be anything on the stack,
			// regardless of any errors encountered while loading the file:
			assert(lua_gettop(luaState) == 0);

			lua_close(luaState);
			luaState = nullptr;
		}

		return luaState;
	}
}
