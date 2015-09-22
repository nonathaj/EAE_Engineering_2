
#include <sstream>
#include <cassert>

#include "LuaHelpers.h"
#include "../../Engine/System/Console.h"

namespace
{
	bool GetDoubleFromTableReady(lua_State* io_luaState, double& o_val);
	bool GetStringFromTableReady(lua_State* io_luaState, std::string& o_val);
}

namespace LuaHelpers
{
	//Is there a table at the top of the stack?
	bool IsTable(lua_State* io_luaStateFrom)
	{
		return lua_istable(io_luaStateFrom, -1);
	}

	template<double>
	double GetFromTable(lua_State* io_luaState, const int& i_indexInTable)
	{
		double value = std::numeric_limits<double>::quiet_NaN();

		if (IsTable(io_luaStateFrom))
		{
			lua_pushinteger(io_luaStateFrom, i_indexInTable);
			lua_gettable(io_luaStateFrom, -2);
			if (lua_isnumber(io_luaStateFrom, -1))
				value = lua_tonumber(io_luaStateFrom, -1);
			lua_pop(io_luaStateFrom, 1);
		}

		return value;
	}

	//Returns a double from a table at the top of the stack
	double GetDoubleFromTable(lua_State* io_luaStateFrom, const int& i_indexInTable)
	{
		double value = std::numeric_limits<double>::quiet_NaN();

		if (IsTable(io_luaStateFrom))
		{
			lua_pushinteger(io_luaStateFrom, i_indexInTable);
			lua_gettable(io_luaStateFrom, -2);
			if (lua_isnumber(io_luaStateFrom, -1))
				value = lua_tonumber(io_luaStateFrom, -1);
			lua_pop(io_luaStateFrom, 1);
		}

		return value;
	}

	double GetDoubleFromTable(lua_State* io_luaStateFrom, const std::string& i_indexInTable)
	{
		double value = std::numeric_limits<double>::quiet_NaN();

		if (IsTable(io_luaStateFrom))
		{
			lua_pushstring(io_luaStateFrom, i_indexInTable.c_str());
			lua_gettable(io_luaStateFrom, -2);
			if (lua_isnumber(io_luaStateFrom, -1))
				value = lua_tonumber(io_luaStateFrom, -1);
			lua_pop(io_luaStateFrom, 1);
		}

		return value;
	}

	std::vector<double> GetDoublesFromArrayTable(lua_State* io_luaState)
	{
		std::vector<double> values;

		if (IsTable(io_luaState))
		{
			size_t value_count = luaL_len(io_luaState, -1);
			for (size_t x = 1; x <= value_count; x++)
				values.push_back(GetDoubleFromTable(io_luaState, x));
		}

		return values;
	}

	std::vector<std::string> GetStringsFromArrayTable(lua_State* io_luaState)
	{
		std::vector<std::string> values;

		if (IsTable(io_luaState))
		{
			size_t value_count = luaL_len(io_luaState, -1);
			for (size_t x = 1; x <= value_count; x++)
				values.push_back(GetStringFromTable(io_luaState, x));
		}

		return values;
	}

	std::string GetStringFromTable(lua_State* io_luaState, const int& i_indexInTable)
	{
		std::string str;

		if (IsTable(io_luaState))
		{
			lua_pushinteger(io_luaState, i_indexInTable);
			lua_gettable(io_luaState, -2);
			if (lua_isnumber(io_luaState, -1))
				str = lua_tostring(io_luaState, -1);
			lua_pop(io_luaState, 1);
		}

		return str;
	}

	std::string GetStringFromTable(lua_State* io_luaState, const std::string& i_indexInTable)
	{
		std::string str;

		if (IsTable(io_luaState))
		{
			lua_pushstring(io_luaState, i_indexInTable.c_str());
			lua_gettable(io_luaState, -2);
			if (lua_isnumber(io_luaState, -1))
				str = lua_tostring(io_luaState, -1);
			lua_pop(io_luaState, 1);
		}

		return str;
	}

	bool PushTable(lua_State* io_luaState, const std::string& i_key)
	{
		lua_pushstring(io_luaState, i_key.c_str());
		lua_gettable(io_luaState, -2);
		if (!IsTable(io_luaState))
		{
			lua_pop(io_luaState, 1);
			return false;
		}
		return true;
	}

	bool PushTable(lua_State* io_luaState, const int& i_key)
	{
		lua_pushinteger(io_luaState, i_key);
		lua_gettable(io_luaState, -2);
		if (!IsTable(io_luaState))
		{
			lua_pop(io_luaState, 1);
			return false;
		}
		return true;
	}

	//Loads the lua table file, and generates a lua_State with the return'd table at the top of the stack
	lua_State* LoadAssetTable(const std::string &i_path)
	{
		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State* luaState = luaL_newstate();
		if (!luaState)
		{
			DEBUG_PRINT("Failed to create a new Lua state\n");
			return false;
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		{
			const int luaResult = luaL_loadfile(luaState, i_path.c_str());
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
				DEBUG_PRINT(lua_tostring(luaState, -1));
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
						DEBUG_PRINT(errorStream.str());
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
					DEBUG_PRINT(errorStream.str());
					// Pop every value that was returned
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				DEBUG_PRINT(lua_tostring(luaState, -1));
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}

		// If this code is reached the asset file was loaded successfully, and its table is now at index -1
		return luaState;

		// Pop the table
		//lua_pop(luaState, 1);

	OnExit:

		if (luaState)
		{
			// If I haven't made any mistakes
			// there shouldn't be anything on the stack,
			// regardless of any errors encountered while loading the file:
			assert(lua_gettop(luaState) == 0);

			lua_close(luaState);
		}

		return nullptr;
	}
}

namespace
{
	bool GetDoubleFromTableReady(lua_State* io_luaState, double& o_val)
	{
		bool found = false;
		if (LuaHelpers::IsTable(io_luaState))
		{
			lua_gettable(io_luaState, -2);
			if (lua_isnumber(io_luaState, -1))
			{
				o_val = lua_tonumber(io_luaState, -1);
				found = true;
			}
			lua_pop(io_luaState, 1);
		}
		return found;
	}

	bool GetStringFromTableReady(lua_State* io_luaState, std::string& o_val)
	{

	}
}