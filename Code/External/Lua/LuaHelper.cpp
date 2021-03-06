
#include <sstream>
#include <cassert>

#include "LuaHelper.h"

namespace
{
	template<typename T>
	bool CheckHasValue(LuaHelper::LuaStack *stack, T i_key, int i_table_index);

	template<typename T>
	bool TryPeekArray(LuaHelper::LuaStack *stack, std::vector<T>& o_val, int i_index);

	template<typename K, typename V>
	bool TryPeekDictionary(LuaHelper::LuaStack *stack, std::unordered_map<K, V>& o_val, int i_index);
}

namespace LuaHelper
{
	LuaStack* LuaStack::Create(const std::string &i_data_path)
	{
		lua_State *state = LoadAssetTable(i_data_path);
		if (!state)
			return nullptr;
		return new LuaStack(state);		
	}

	//Is there a table at the top of the stack?
	bool LuaStack::IsTable(int i_index)
	{
		return lua_istable(state, i_index);
	}

	bool LuaStack::IsNumber(int i_index)
	{
		return lua_isnumber(state, i_index) != 0;
	}

	bool LuaStack::IsNil(int i_index)
	{
		return lua_isnil(state, i_index);
	}

	bool LuaStack::IsString(int i_index)
	{
		return lua_isstring(state, i_index) != 0;
	}

	bool LuaStack::IsBoolean(int i_index)
	{
		return lua_isboolean(state, i_index);
	}

	size_t LuaStack::TableLength(int i_index)
	{
		return luaL_len(state, i_index);
	}

	void LuaStack::Pop(int i_amount)
	{
		lua_pop(state, i_amount);
	}

	LuaStack::~LuaStack()
	{
		if (state)
		{
			lua_close(state);
			state = nullptr;
		}
	}

	bool LuaStack::SwapTableKey(int i_table_index)
	{
		if (IsTable(i_table_index))
		{
			lua_gettable(state, i_table_index);
			return true;
		}
		return false;
	}

	////////////////////////////////////
	// Push Functions
	////////////////////////////////////
	void LuaStack::Push(lua_Integer const& i_val)
	{
		lua_pushinteger(state, i_val);
	}

	void LuaStack::Push(const char* i_val)
	{
		lua_pushstring(state, i_val);
	}

	void LuaStack::Push(nullptr_t const& i_val)
	{
		lua_pushnil(state);
	}

	void LuaStack::Push(lua_Number const& i_val)
	{
		lua_pushnumber(state, i_val);
	}

	void LuaStack::Push(lua_Unsigned const& i_val)
	{
		lua_pushunsigned(state, i_val);
	}

	void LuaStack::Push(bool const& i_val)
	{
		lua_pushboolean(state, i_val);
	}
	////////////////////////////////////
	////////////////////////////////////

	////////////////////////////////////
	// Peek Functions
	////////////////////////////////////
	bool LuaStack::Peek(lua_Number& o_val, int i_index)
	{
		if (IsNumber(i_index))
		{
			o_val = lua_tonumber(state, i_index);
			return true;
		}
		return false;
	}

	bool LuaStack::Peek(std::string& o_val, int i_index)
	{
		if (IsString(i_index))
		{
			o_val = lua_tostring(state, i_index);
			return true;
		}
		return false;
	}

	bool LuaStack::Peek(bool& o_val, int i_index)
	{
		if (IsBoolean(i_index))
		{
			o_val = lua_toboolean(state, i_index) != 0;
			return true;
		}
		return false;
	}

	bool LuaStack::Peek(lua_Integer& o_val, int i_index)
	{
		if (IsNumber(i_index))
		{
			o_val = lua_tointeger(state, i_index);
			return true;
		}
		return false;
	}

	bool LuaStack::Peek(lua_Unsigned & o_val, int i_index)
	{
		if (IsNumber(i_index))
		{
			o_val = lua_tounsigned(state, i_index);
			return true;
		}
		return false;
	}

	////////////////////////////////////
	// HasValue Functions
	////////////////////////////////////

#define LUAHELPER_LUASTACK_HASVALUE_DEFINITION(KeyType) \
	bool LuaStack::HasValue(KeyType i_key, int i_table_index) \
	{ \
		return CheckHasValue(this, i_key, i_table_index); \
	}

	LUAHELPER_LUASTACK_HASVALUE_DEFINITION(const char*);
	LUAHELPER_LUASTACK_HASVALUE_DEFINITION(lua_Number const&);
	LUAHELPER_LUASTACK_HASVALUE_DEFINITION(lua_Integer const&);
	LUAHELPER_LUASTACK_HASVALUE_DEFINITION(lua_Unsigned const&);

#undef LUAHELPER_LUASTACK_HASVALUE_DEFINITION

	////////////////////////////////////
	////////////////////////////////////

	////////////////////////////////////
	// Pop Array Functions
	////////////////////////////////////

#define LUAHELPER_LUASTACK_PEEKARRAY_DEFINITION(StoreType) \
	bool LuaStack::PeekArray(std::vector<StoreType>& o_val, int i_index) \
	{ \
		return TryPeekArray(this, o_val, i_index); \
	}

	LUAHELPER_LUASTACK_PEEKARRAY_DEFINITION(std::string);
	LUAHELPER_LUASTACK_PEEKARRAY_DEFINITION(lua_Integer);
	LUAHELPER_LUASTACK_PEEKARRAY_DEFINITION(lua_Number);
	LUAHELPER_LUASTACK_PEEKARRAY_DEFINITION(lua_Unsigned);

#undef LUAHELPER_LUASTACK_PEEKARRAY_DEFINITION

	////////////////////////////////////
	////////////////////////////////////

	////////////////////////////////////
	// Pop Dictionary Functions
	////////////////////////////////////

#define LUAHELPER_LUASTACK_PEEKDICTIONARY_DEFINITION(KeyType, ValueType) \
bool LuaStack::PeekDictionary(std::unordered_map<KeyType, ValueType>& o_val, int i_index) \
{ \
	return TryPeekDictionary(this, o_val, i_index); \
}

	LUAHELPER_LUASTACK_PEEKDICTIONARY_DEFINITION(std::string, lua_Integer);
	LUAHELPER_LUASTACK_PEEKDICTIONARY_DEFINITION(std::string, std::string);
	LUAHELPER_LUASTACK_PEEKDICTIONARY_DEFINITION(std::string, lua_Number);
	LUAHELPER_LUASTACK_PEEKDICTIONARY_DEFINITION(std::string, lua_Unsigned);
	LUAHELPER_LUASTACK_PEEKDICTIONARY_DEFINITION(std::string, bool);

#undef LUAHELPER_LUASTACK_PEEKDICTIONARY_DEFINITION

	////////////////////////////////////
	////////////////////////////////////

	//Loads the lua table file, and generates a lua_State with the return'd table at the top of the stack
	lua_State* LoadAssetTable(const std::string &i_path, std::function<void(std::string)> error)
	{
		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State* luaState = luaL_newstate();
		if (!luaState)
		{
			if (error)
				error("Failed to create a new Lua state");
			return nullptr;
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		{
			const int luaResult = luaL_loadfile(luaState, i_path.c_str());
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
				if (error)
					error(lua_tostring(luaState, -1));
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
						if (error)
						{
							std::stringstream errorStream;
							errorStream << "Asset files must return a table (instead of a " << luaL_typename(luaState, -1) << ")";
							error(errorStream.str());
						}
						// Pop the returned non-table value
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					if (error)
					{
						std::stringstream errorStream;
						errorStream << "Asset files must return a single table (instead of " << returnedValueCount << " values)";
						error(errorStream.str());
					}
					// Pop every value that was returned
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				if (error)
					error(lua_tostring(luaState, -1));
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}

		// If this code is reached the asset file was loaded successfully, and its table is now at index -1
		if (lua_istable(luaState, -1))
			return luaState;

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
		}

		return nullptr;
	}

}

namespace
{
	template<typename T>
	bool CheckHasValue(LuaHelper::LuaStack *stack, T i_key, int i_table_index)
	{
		if (!stack->IsTable(i_table_index))
			return false;

		stack->Push(i_key);
		bool hasValue = stack->SwapTableKey(i_table_index - 1) && !stack->IsNil();
		stack->Pop();
		return hasValue;
	}

	template<typename T>
	bool TryPeekArray(LuaHelper::LuaStack *stack, std::vector<T>& o_val, int i_index)
	{
		if (!stack->IsTable(i_index))
			return false;

		T item;
		size_t value_count = stack->TableLength(i_index);
		for (size_t x = 1; x <= value_count; x++)
		{
			stack->Push(static_cast<lua_Unsigned>(x));					//push the key
			if(stack->SwapTableKey(i_index - 1) && stack->Peek(item))	//swap for value, and try to get it
				o_val.push_back(item);
			stack->Pop();												//pop the key				
		}
		return true;
	}

	template<typename K, typename V>
	bool TryPeekDictionary(LuaHelper::LuaStack *stack, std::unordered_map<K, V>& o_val, int i_index)
	{
		if (!stack->IsTable(i_index))
			return false;

		K key;
		V val;
		for (stack->Push(nullptr); lua_next(stack->State(), i_index - 1); stack->Pop())
		{
			if (stack->Peek(val, -1) && stack->Peek(key, -2))
				o_val[key] = val;
		}
		return true;
	}
}
