#ifndef _EXTERNAL_LUA_LUAHELPER_H
#define _EXTERNAL_LUA_LUAHELPER_H

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "Includes.h"

namespace LuaHelper
{
	//Is there a table at a position on the stack? (top of the stack by default)
	bool IsTable(lua_State* io_luaStateFrom, int i_index = -1);

	//Gets the array length of the table at the top of the stack
	size_t TableLength(lua_State* io_luaState, int i_index = -1);
	size_t DictionaryLength(lua_State* io_luaState, int i_index = -1);

	//Pushes a value on top of the lua stack
	void Push(lua_State* io_luaState, lua_Integer const& i_val);
	void Push(lua_State* io_luaState, std::string const& i_val);
	void Push(lua_State* io_luaState, nullptr_t const& i_val);			//push nil
	void Push(lua_State* io_luaState, lua_Number const& i_val);
	void Push(lua_State* io_luaState, lua_Unsigned const& i_val);
	void Push(lua_State* io_luaState, bool const& i_val);

	//Peeks at the item at the top of the stack
	bool Peek(lua_State* io_luaState, lua_Number& o_val);
	bool Peek(lua_State* io_luaState, std::string& o_val);
	bool Peek(lua_State* io_luaState, bool& o_val);
	bool Peek(lua_State* io_luaState, lua_Integer& o_val);
	bool Peek(lua_State* io_luaState, lua_Unsigned & o_val);
	bool Peek(lua_State* io_luaState, nullptr_t const& i_val);			//is the top of the stack nil?

	void Pop(lua_State* io_luaState, int i_amount = 1);				//Pops items off the top of the stack

	//Swaps a key at the top of the stack with it's value in the table right below it in the stack
	bool SwapTableKey(lua_State* io_luaState);

	//Gets an array from the top of the stack, with the given index key, and pops it off (item is ALWAYS popped off, even if failure)
	bool PopArray(lua_State* io_luaState, std::vector<lua_Integer>& o_val);
	bool PopArray(lua_State* io_luaState, std::vector<std::string>& o_val);
	bool PopArray(lua_State* io_luaState, std::vector<lua_Number>& o_val);
	bool PopArray(lua_State* io_luaState, std::vector<lua_Unsigned>& o_val);

	//Gets a dictionary from the top of the stack, with the given index key, and pops it off (item is ALWAYS popped off, even if failure)
	bool PopDictionary(lua_State* io_luaState, std::map<std::string, lua_Integer>& o_val);
	bool PopDictionary(lua_State* io_luaState, std::map<std::string, std::string>& o_val);
	bool PopDictionary(lua_State* io_luaState, std::map<std::string, lua_Number>& o_val);
	bool PopDictionary(lua_State* io_luaState, std::map<std::string, lua_Unsigned>& o_val);

	//Loads the lua table file, and generates a lua_State with the return'd table at the top of the stack
	lua_State* LoadAssetTable(const std::string& i_path, std::function<void(std::string)> error = nullptr);

	bool Close(lua_State*& io_luaState);
}

#endif //_EXTERNAL_LUA_LUAHELPER_H