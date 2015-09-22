#ifndef _EXTERNAL_LUA_LUAHELPERS_H
#define _EXTERNAL_LUA_LUAHELPERS_H

#include <string>
#include <vector>

#include "Includes.h"

namespace LuaHelpers
{
	//Is there a table at the top of the stack?
	bool IsTable(lua_State* io_luaStateFrom);

	template<typename T> 
	T GetFromTable(lua_State* io_luaState, const int& i_indexInTable);

	//Returns a double from a table at the top of the stack (NaN, if the value is not a lua number)
	double GetDoubleFromTable(lua_State* io_luaStateFrom, const int& i_indexInTable);
	double GetDoubleFromTable(lua_State* io_luaStateFrom, const std::string& i_indexInTable);

	std::string GetStringFromTable(lua_State* io_luaState, const int& i_indexInTable);
	std::string GetStringFromTable(lua_State* io_luaState, const std::string& i_indexInTable);

	std::vector<double> GetDoublesFromArrayTable(lua_State* io_luaState);
	std::vector<std::string> GetStringsFromArrayTable(lua_State* io_luaState);

	//attempts to push a new table on the stack with the given key
	bool PushTable(lua_State* io_luaState, const std::string& i_key);
	bool PushTable(lua_State* io_luaState, const int& i_key);

	//Loads the lua table file, and generates a lua_State with the return'd table at the top of the stack
	lua_State* LoadAssetTable(const std::string& i_path);
}

#endif //_EXTERNAL_LUA_LUAHELPERS_H