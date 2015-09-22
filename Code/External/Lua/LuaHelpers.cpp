
#include <sstream>
#include <cassert>

#include "LuaHelpers.h"
#include "../../Engine/System/Console.h"

namespace LuaHelpers
{
	//Is there a table at the top of the stack?
	bool IsTable(lua_State* io_luaStateFrom)
	{
		return lua_istable(io_luaStateFrom, -1);
	}

	size_t TableLength(lua_State* io_luaState)
	{
		return luaL_len(io_luaState, -1);
	}

	void Pop(lua_State* io_luaState, size_t i_amount)
	{
		lua_pop(io_luaState, i_amount);
	}

	bool Close(lua_State*& io_luaState)
	{
		if (!io_luaState)
			return false;

		assert(lua_gettop(io_luaState) == 0);

		lua_close(io_luaState);
		io_luaState = nullptr;
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

	////////////////////////////////////
	// Push Functions
	////////////////////////////////////
	template<>
	void Push(lua_State* io_luaState, lua_Integer const& i_val)
	{
		lua_pushinteger(io_luaState, i_val);
	}

	template<>
	void Push(lua_State* io_luaState, std::string const& i_val)
	{
		lua_pushstring(io_luaState, i_val.c_str());
	}

	template<>
	void Push(lua_State* io_luaState, char const * const& i_val)
	{
		lua_pushstring(io_luaState, i_val);
	}

	template<>
	void Push(lua_State* io_luaState, nullptr_t const& i_val)
	{
		lua_pushnil(io_luaState);
	}

	template<>
	void Push(lua_State* io_luaState, lua_Number const& i_val)
	{
		lua_pushnumber(io_luaState, i_val);
	}

	template<>
	void Push(lua_State* io_luaState, lua_Unsigned const& i_val)
	{
		lua_pushunsigned(io_luaState, i_val);
	}

	void Push(lua_State* io_luaState, const char* i_val)
	{
		lua_pushstring(io_luaState, i_val);
	}
	////////////////////////////////////
	////////////////////////////////////

	////////////////////////////////////
	// Peek Functions
	////////////////////////////////////
	template<>
	bool Peek(lua_State* io_luaState, lua_Number& o_val)
	{
		if (lua_isnumber(io_luaState, -1))
		{
			o_val = lua_tonumber(io_luaState, -1);
			return true;
		}
		return false;
	}

	template<>
	bool Peek(lua_State* io_luaState, std::string& o_val)
	{
		if (lua_isstring(io_luaState, -1))
		{
			o_val = lua_tostring(io_luaState, -1);
			return true;
		}
		return false;
	}

	template<>
	bool Peek(lua_State* io_luaState, const char*& o_val)
	{
		if (lua_isstring(io_luaState, -1))
		{
			o_val = lua_tostring(io_luaState, -1);
			return true;
		}
		return false;
	}

	template<>
	bool Peek(lua_State* io_luaState, bool& o_val)
	{
		if (lua_isboolean(io_luaState, -1))
		{
			o_val = lua_toboolean(io_luaState, -1) != 0;
			return true;
		}
		return false;
	}

	template<>
	bool Peek(lua_State* io_luaState, lua_Integer& o_val)
	{
		if (lua_isnumber(io_luaState, -1))
		{
			o_val = lua_tointeger(io_luaState, -1);
			return true;
		}
		return false;
	}

	////////////////////////////////////
	////////////////////////////////////

	template<typename T>
	bool PopValue(lua_State* io_luaState, T& o_val)
	{
		bool pass = Peek(io_luaState, o_val);
		Pop(io_luaState);
		return pass;
	}

	template<typename K, typename T>
	bool GetFromTable(lua_State* io_luaState, const K& i_indexInTable, T& o_val)
	{
		bool found = false;
		if (IsTable(io_luaState))
		{
			Push(io_luaState, i_indexInTable);
			lua_gettable(io_luaState, -2);
			found = PopValue(io_luaState, o_val);
		}
		return found;
	}


	template<typename T>
	bool PopArray(lua_State* io_luaState, std::vector<T>& o_val)
	{
		if (!IsTable(io_luaState))
		{
			Pop(io_luaState);
			return false;
		}

		T item;
		size_t value_count = TableLength(io_luaState);
		o_val.reserve(value_count);
		for (size_t x = 1; x <= value_count; x++)
		{
			if (GetFromTable(io_luaState, x, item))
				o_val.push_back(item);
			else
			{
				Pop(io_luaState);
				return false;
			}
		}
		Pop(io_luaState);
		return true;
	}

	template<typename K, typename T>
	bool PopDictionary(lua_State* io_luaState, std::map<K, T> o_val)
	{
		if (!IsTable(io_luaState))
		{
			Pop(io_luaState);
			return false;
		}

		K key;
		T val;
		Push(io_luaState, nullptr);
		while (lua_next(io_luaState, -2))
		{
			if (Pop(io_luaState, val) && Pop(io_luaState, key))
				o_val.insert(key, val);
			else
			{
				Pop(io_luaState);
				return false;
			}
		}
		Pop(io_luaState);
		return true;
	}

	template<typename T>
	bool PushTable(lua_State* io_luaState, const T& i_key)
	{
		if (!IsTable(io_luaState))
			return false;

		Push(io_luaState, i_key);
		lua_gettable(io_luaState, -2);
		if (!IsTable(io_luaState))
		{
			Pop(io_luaState);
			return false;
		}
		return true;
	}
}
