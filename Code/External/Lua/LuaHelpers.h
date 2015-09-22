#ifndef _EXTERNAL_LUA_LUAHELPERS_H
#define _EXTERNAL_LUA_LUAHELPERS_H

#include <string>
#include <vector>
#include <map>

#include "Includes.h"

namespace LuaHelpers
{
	//Is there a table at the top of the stack?
	bool IsTable(lua_State* io_luaStateFrom);

	//Pushes a value on top of the lua stack
	template<typename T>
	void Push(lua_State* io_luaState, const T& i_val);

	//Peeks at the item at the top of the stack
	template<typename T>
	bool Peek(lua_State* io_luaState, T& o_val);

	//Pops at the item at the top of the stack
	template<typename T>
	bool PopValue(lua_State* io_luaState, T& o_val);

	void Pop(lua_State* io_luaState, size_t i_amount = 1);

	//Gets a value from a table at the top of the stack, with the given index key
	template<typename K, typename T>
	bool GetFromTable(lua_State* io_luaState, const K& i_indexInTable, T& o_val);

	//Gets an array from the top of the stack, with the given index key, and pops it off (item is ALWAYS popped off, even if failure)
	template<typename T>
	bool PopArray(lua_State* io_luaState, std::vector<T>& o_val);

	//Gets a dictionary from the top of the stack, with the given index key, and pops it off (item is ALWAYS popped off, even if failure)
	template<typename K, typename T>
	bool PopDictionary(lua_State* io_luaState, std::map<K, T> o_val);

	//attempts to push a new table on the stack with the given key
	template<typename T>
	bool PushTable(lua_State* io_luaState, const T& i_key);

	//Loads the lua table file, and generates a lua_State with the return'd table at the top of the stack
	lua_State* LoadAssetTable(const std::string& i_path);

	size_t TableLength(lua_State* io_luaState);

	bool Close(lua_State*& io_luaState);
}
#include "LuaHelpers.inl"

#endif //_EXTERNAL_LUA_LUAHELPERS_H
