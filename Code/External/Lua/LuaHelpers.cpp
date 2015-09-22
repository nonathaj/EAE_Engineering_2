
#include <sstream>
#include <cassert>

#include "LuaHelpers.h"
#include "../../Engine/System/Console.h"

namespace LuaHelpers
{
	//Is there a table at the top of the stack?
	bool IsTable(lua_State* io_luaStateFrom, int i_index)
	{
		return lua_istable(io_luaStateFrom, i_index);
	}

	size_t TableLength(lua_State* io_luaState, int i_index)
	{
		return luaL_len(io_luaState, i_index);
	}

	size_t DictionaryLength(lua_State* io_luaState, int i_index)
	{
		bool success = IsTable(io_luaState);
		if (success)
		{
			size_t count = 0;
			std::string key;
			lua_Unsigned val;
			Push(io_luaState, nullptr);
			while (lua_next(io_luaState, -2))	//pushes key, then value on the stack
			{
				Pop(io_luaState);		//pop the value
				count++;
			}
			return count;
		}
		return success;
	}

	void Pop(lua_State* io_luaState, int i_amount)
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

	bool SwapTableKey(lua_State* io_luaState)
	{
		if (IsTable(io_luaState, -2))
		{
			lua_gettable(io_luaState, -2);
			return true;
		}
		return false;
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
			return nullptr;
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
						DEBUG_PRINT("Asset files must return a table (instead of a %s", luaL_typename(luaState, -1));
						// Pop the returned non-table value
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					DEBUG_PRINT("Asset files must return a single table (instead of %d values)", returnedValueCount);
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
		if(IsTable(luaState))
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
	void Push(lua_State* io_luaState, lua_Integer const& i_val)
	{
		lua_pushinteger(io_luaState, i_val);
	}

	void Push(lua_State* io_luaState, std::string const& i_val)
	{
		lua_pushstring(io_luaState, i_val.c_str());
	}

	void Push(lua_State* io_luaState, nullptr_t const& i_val)
	{
		lua_pushnil(io_luaState);
	}

	void Push(lua_State* io_luaState, lua_Number const& i_val)
	{
		lua_pushnumber(io_luaState, i_val);
	}

	void Push(lua_State* io_luaState, lua_Unsigned const& i_val)
	{
		lua_pushunsigned(io_luaState, i_val);
	}

	void Push(lua_State* io_luaState, bool const& i_val)
	{
		lua_pushboolean(io_luaState, i_val);
	}
	////////////////////////////////////
	////////////////////////////////////

	////////////////////////////////////
	// Peek Functions
	////////////////////////////////////
	bool Peek(lua_State* io_luaState, lua_Number& o_val)
	{
		if (lua_isnumber(io_luaState, -1))
		{
			o_val = lua_tonumber(io_luaState, -1);
			return true;
		}
		return false;
	}

	bool Peek(lua_State* io_luaState, std::string& o_val)
	{
		if (lua_isstring(io_luaState, -1))
		{
			o_val = lua_tostring(io_luaState, -1);
			return true;
		}
		return false;
	}

	bool Peek(lua_State* io_luaState, bool& o_val)
	{
		if (lua_isboolean(io_luaState, -1))
		{
			o_val = lua_toboolean(io_luaState, -1) != 0;
			return true;
		}
		return false;
	}

	bool Peek(lua_State* io_luaState, lua_Integer& o_val)
	{
		if (lua_isnumber(io_luaState, -1))
		{
			o_val = lua_tointeger(io_luaState, -1);
			return true;
		}
		return false;
	}

	bool Peek(lua_State* io_luaState, lua_Unsigned & o_val)
	{
		if (lua_isnumber(io_luaState, -1))
		{
			o_val = lua_tounsigned(io_luaState, -1);
			return true;
		}
		return false;
	}

	bool Peek(lua_State* io_luaState, nullptr_t const& i_val)
	{
		return lua_isnil(io_luaState, -1);
	}

	////////////////////////////////////
	////////////////////////////////////

	////////////////////////////////////
	// Pop Array Functions
	////////////////////////////////////

	bool PopArray(lua_State* io_luaState, std::vector<lua_Integer>& o_val)
	{
		if (!IsTable(io_luaState))
		{
			Pop(io_luaState);
			return false;
		}

		lua_Integer item;
		size_t value_count = TableLength(io_luaState);
		o_val.reserve(value_count);
		for (size_t x = 1; x <= value_count; x++)
		{
			Push(io_luaState, static_cast<lua_Unsigned>(x));												//push the key
			bool found = SwapTableKey(io_luaState) && Peek(io_luaState, item);	//swap for value, and try to get it
			Pop(io_luaState);													//pop the key
			if (found)
				o_val.push_back(item);
			else
				return false;
		}
		Pop(io_luaState);	//pop the table off the top of the stack
		return true;
	}

	bool PopArray(lua_State* io_luaState, std::vector<std::string>& o_val)
	{
		if (!IsTable(io_luaState))
		{
			Pop(io_luaState);
			return false;
		}

		std::string item;
		size_t value_count = TableLength(io_luaState);
		o_val.reserve(value_count);
		for (size_t x = 1; x <= value_count; x++)
		{
			Push(io_luaState, static_cast<lua_Unsigned>(x));										//push the key
			bool found = SwapTableKey(io_luaState) && Peek(io_luaState, item);	//swap for value, and try to get it
			Pop(io_luaState);													//pop the key
			if (found)
				o_val.push_back(item);
			else
				return false;
		}
		Pop(io_luaState);	//pop the table off the top of the stack
		return true;
	}

	bool PopArray(lua_State* io_luaState, std::vector<lua_Number>& o_val)
	{
		if (!IsTable(io_luaState))
		{
			Pop(io_luaState);
			return false;
		}

		lua_Number item;
		size_t value_count = TableLength(io_luaState);
		o_val.reserve(value_count);
		for (size_t x = 1; x <= value_count; x++)
		{
			Push(io_luaState, static_cast<lua_Unsigned>(x));												//push the key
			bool found = SwapTableKey(io_luaState) && Peek(io_luaState, item);	//swap for value, and try to get it
			Pop(io_luaState);													//pop the key
			if (found)
				o_val.push_back(item);
			else
				return false;
		}
		Pop(io_luaState);	//pop the table off the top of the stack
		return true;
	}

	bool PopArray(lua_State* io_luaState, std::vector<lua_Unsigned>& o_val)
	{
		if (!IsTable(io_luaState))
		{
			Pop(io_luaState);
			return false;
		}

		lua_Unsigned item;
		size_t value_count = TableLength(io_luaState);
		o_val.reserve(value_count);
		for (size_t x = 1; x <= value_count; x++)
		{
			Push(io_luaState, static_cast<lua_Unsigned>(x));												//push the key
			bool found = SwapTableKey(io_luaState) && Peek(io_luaState, item);	//swap for value, and try to get it
			Pop(io_luaState);													//pop the key
			if (found)
				o_val.push_back(item);
			else
				return false;
		}
		Pop(io_luaState);	//pop the table off the top of the stack
		return true;
	}

	////////////////////////////////////
	////////////////////////////////////

	////////////////////////////////////
	// Pop Dictionary Functions
	////////////////////////////////////
	bool PopDictionary(lua_State* io_luaState, std::map<std::string, lua_Integer>& o_val)
	{
		bool success = IsTable(io_luaState);
		if (success)
		{
			std::string key;
			lua_Integer val;
			Push(io_luaState, nullptr);
			while (lua_next(io_luaState, -2))	//pushes key, then value on the stack
			{
				bool poppedSuccessfully = Peek(io_luaState, val);
				Pop(io_luaState);		//pop the value
				poppedSuccessfully = poppedSuccessfully && Peek(io_luaState, key);
				Pop(io_luaState);		//pop the key
				if (poppedSuccessfully)
					o_val[key] = val;
				else
				{
					success = false;
					break;
				}
			}
		}
		Pop(io_luaState);
		return success;
	}

	bool PopDictionary(lua_State* io_luaState, std::map<std::string, std::string>& o_val)
	{
		bool success = IsTable(io_luaState);
		if (success)
		{
			std::string key;
			std::string val;
			Push(io_luaState, nullptr);
			while (lua_next(io_luaState, -2))	//pushes key, then value on the stack
			{
				bool poppedSuccessfully = Peek(io_luaState, val);
				Pop(io_luaState);		//pop the value
				poppedSuccessfully = poppedSuccessfully && Peek(io_luaState, key);
				Pop(io_luaState);		//pop the key
				if (poppedSuccessfully)
					o_val[key] = val;
				else
				{
					success = false;
					break;
				}
			}
		}
		Pop(io_luaState);
		return success;
	}

	bool PopDictionary(lua_State* io_luaState, std::map<std::string, lua_Number>& o_val)
	{
		bool success = IsTable(io_luaState);
		if (success)
		{
			std::string key;
			lua_Number val;
			Push(io_luaState, nullptr);
			while (lua_next(io_luaState, -2))	//pushes key, then value on the stack
			{
				bool poppedSuccessfully = Peek(io_luaState, val);
				Pop(io_luaState);		//pop the value
				poppedSuccessfully = poppedSuccessfully && Peek(io_luaState, key);
				Pop(io_luaState);		//pop the key
				if (poppedSuccessfully)
					o_val[key] = val;
				else
				{
					success = false;
					break;
				}
			}
		}
		Pop(io_luaState);
		return success;
	}

	bool PopDictionary(lua_State* io_luaState, std::map<std::string, lua_Unsigned>& o_val)
	{
		bool success = IsTable(io_luaState);
		if (success)
		{
			std::string key;
			lua_Unsigned val;
			Push(io_luaState, nullptr);
			while (lua_next(io_luaState, -2))	//pushes key, then value on the stack
			{
				bool poppedSuccessfully = Peek(io_luaState, val);
				Pop(io_luaState);		//pop the value
				poppedSuccessfully = poppedSuccessfully && Peek(io_luaState, key);
				Pop(io_luaState);		//pop the key
				if (poppedSuccessfully)
					o_val[key] = val;
				else
				{
					success = false;
					break;
				}
			}
		}
		Pop(io_luaState);
		return success;
	}

	////////////////////////////////////
	////////////////////////////////////
}
