
#include "EffectBuilder.h"

#include <sstream>
#include <cassert>
#include <fstream>

#include "../BuilderHelper/UtilityFunctions.h"
#include "../../Engine/Windows/Functions.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Graphics/Effect.h"

namespace
{
	bool LoadEffectAssetTableFromLua(const std::string &i_path, std::string& o_vertex, std::string& o_fragment);
	bool LoadEffect(lua_State* io_luaStateFrom, std::string& o_vertex, std::string& o_fragment);
	bool LoadStringFromTable(lua_State* io_luaStateFrom, const std::string& i_key, std::string& o_value);
}

bool EffectBuilder::Build(const std::vector<std::string>& )
{
	std::string vertex, fragment;

	if (LoadEffectAssetTableFromLua(m_path_source, vertex, fragment))
	{
		//Add the relative folder location of these built assets
		std::string relativeFolder = "data/";
		vertex = relativeFolder + vertex;
		fragment = relativeFolder + fragment;

		//find the length of the vertex string
		uint8_t vertexStringLength = static_cast<uint8_t>(vertex.length());
		
		//warn about vertex string length being too long
		if (vertex.length() > std::numeric_limits<uint8_t>::max())
		{
			std::stringstream error;
			error << "Length of vertex path for effect " << m_path_source << " is greater than " 
				<< sizeof(vertexStringLength) << " (actual value=" << vertex.length() << ")";
			eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
			return false;
		}

		std::ofstream out(m_path_target, std::ofstream::binary);

		if (out)
		{
			//write the data
			out.write(reinterpret_cast<const char *>(&vertexStringLength), sizeof(vertexStringLength));
			out.write(vertex.c_str(), vertexStringLength + 1);
			out.write(fragment.c_str(), fragment.length() + 1);

			out.close();

			return true;
		}
		else
		{
			eae6320::OutputErrorMessage("Failed to open the output file for writing", m_path_source);
			return false;
		}
	}
	else
		return false;
}

namespace
{
	bool LoadStringFromTable(lua_State* io_luaStateFrom, const std::string& i_key, std::string& o_value)
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

	bool LoadEffect(lua_State* io_luaStateFrom, std::string& o_vertex, std::string& o_fragment)
	{
		return LoadStringFromTable(io_luaStateFrom, "vertex", o_vertex) && LoadStringFromTable(io_luaStateFrom, "fragment", o_fragment);
	}

	bool LoadEffectAssetTableFromLua(const std::string &i_path, std::string& o_vertex, std::string& o_fragment)
	{
		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State* luaState = luaL_newstate();
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

		// If this code is reached the asset file was loaded successfully, and its table is now at index -1
		if (!LoadEffect(luaState, o_vertex, o_fragment))
		{
			eae6320::OutputErrorMessage("Failed to load the effect");
			wereThereErrors = true;
		}

		// Pop the table
		lua_pop(luaState, 1);

	OnExit:

		if (luaState)
		{
			// If I haven't made any mistakes
			// there shouldn't be anything on the stack,
			// regardless of any errors encountered while loading the file:
			assert(lua_gettop(luaState) == 0);

			lua_close(luaState);
			luaState = nullptr;
		}

		return !wereThereErrors;
	}
}
