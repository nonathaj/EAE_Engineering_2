#ifndef _EXTERNAL_LUA_LUAHELPER_H
#define _EXTERNAL_LUA_LUAHELPER_H

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "Includes.h"

namespace LuaHelper
{
	//Loads the lua table file, and generates a lua_State with the return'd table at the top of the stack
	static lua_State* LoadAssetTable(const std::string& i_path, std::function<void(std::string)> error = nullptr);

	class LuaStack
	{
	public:
		static LuaStack* Create(const std::string &i_data_path);

		LuaStack(lua_State *i_state) : state(i_state) {}
		~LuaStack();

		//Is there a <type> at a position on the stack? (top of the stack by default)
		bool IsTable(int i_index = -1);
		bool IsNumber(int i_index = -1);
		bool IsNil(int i_index = -1);
		bool IsString(int i_index = -1);
		bool IsBoolean(int i_index = -1);

		//Gets the array length of the table at the top of the stack
		size_t TableLength(int i_index = -1);

		//Pushes a value on top of the lua stack
		void Push(lua_Integer const& i_val);
		void Push(const char* i_val);
		void Push(nullptr_t const& i_val);			//push nil
		void Push(lua_Number const& i_val);
		void Push(lua_Unsigned const& i_val);
		void Push(bool const& i_val);

		//Peeks at the item at the top of the stack
		bool Peek(lua_Number& o_val, int i_index = -1);
		bool Peek(std::string& o_val, int i_index = -1);
		bool Peek(bool& o_val, int i_index = -1);
		bool Peek(lua_Integer& o_val, int i_index = -1);
		bool Peek(lua_Unsigned & o_val, int i_index = -1);

		void Pop(int i_amount = 1);				//Pops items off the top of the stack

		//Swaps a key at the top of the stack with it's value in the table on the stack
		bool SwapTableKey(int i_table_index = -2);

		//Gets the values from an array on the stack, fails if the index is not a table
		bool PeekArray(std::vector<lua_Integer>& o_val, int i_index = -1);
		bool PeekArray(std::vector<std::string>& o_val, int i_index = -1);
		bool PeekArray(std::vector<lua_Number>& o_val, int i_index = -1);
		bool PeekArray(std::vector<lua_Unsigned>& o_val, int i_index = -1);

		//Gets the values from a dictionary on the stack, fails if the index is not a table
		bool PeekDictionary(std::map<std::string, lua_Integer>& o_val, int i_index = -1);
		bool PeekDictionary(std::map<std::string, std::string>& o_val, int i_index = -1);
		bool PeekDictionary(std::map<std::string, lua_Number>& o_val, int i_index = -1);
		bool PeekDictionary(std::map<std::string, lua_Unsigned>& o_val, int i_index = -1);
		bool PeekDictionary(std::map<std::string, bool>& o_val, int i_index = -1);

		lua_State* State() { return state; }

	private:
		lua_State *state;
	};
}

#endif //_EXTERNAL_LUA_LUAHELPER_H
