
#include <iostream>
#include <cassert>
#include <sstream>
#include <functional>

#include "Mesh.h"
#include "../../External/Lua/Includes.h"
#include "../System/Console.h"

#include "Vertex.h"

namespace
{
	bool LoadAssetFromLua(const std::string &i_path, std::function<void(lua_State&)> i_loadTableValuesCallback);
	void LoadMesh(lua_State &i_luaStateFrom, Lame::Vertex*& o_vertices, size_t& o_vertex_count, uint32_t*& o_indices, size_t& o_index_count);
}

namespace Lame
{
	Mesh* Mesh::Create(std::string i_mesh_path)
	{
		Vertex *vertices;
		size_t vertex_count;

		uint32_t *indices;
		size_t index_count;

		LoadAssetFromLua(i_mesh_path, std::bind(LoadMesh, std::placeholders::_1, vertices, vertex_count, indices, index_count));

		return CreateRightHanded(vertices, vertex_count, indices, index_count);
	}
}

namespace
{
	void LoadMesh(lua_State &i_luaStateFrom, Lame::Vertex*& o_vertices, size_t& o_vertex_count, uint32_t*& o_indices, size_t& o_index_count)
	{
		//TODO
		//Load the vertex and index data and apply it to the provided arrays/sizes
	}

	bool LoadAssetFromLua(const std::string &i_path, std::function<void(lua_State&)> i_loadTableValuesCallback)
	{
		bool wereThereErrors = false;

		// Create a new Lua state
		lua_State* luaState = NULL;
		{
			luaState = luaL_newstate();
			if (!luaState)
			{
				wereThereErrors = true;
				std::cerr << "Failed to create a new Lua state"
					"\n";
				goto OnExit;
			}
		}

		// Load the asset file as a "chunk",
		// meaning there will be a callable function at the top of the stack
		{
			const int luaResult = luaL_loadfile(luaState, i_path.c_str());
			if (luaResult != LUA_OK)
			{
				wereThereErrors = true;
				std::cerr << lua_tostring(luaState, -1);
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
						std::cerr << "Asset files must return a table (instead of a " <<
							luaL_typename(luaState, -1) << ")\n";
						// Pop the returned non-table value
						lua_pop(luaState, 1);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::cerr << "Asset files must return a single table (instead of " <<
						returnedValueCount << " values)"
						"\n";
					// Pop every value that was returned
					lua_pop(luaState, returnedValueCount);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::cerr << lua_tostring(luaState, -1);
				// Pop the error message
				lua_pop(luaState, 1);
				goto OnExit;
			}
		}


		// If this code is reached the asset file was loaded successfully,
		// and its table is now at index -1
		if(i_loadTableValuesCallback)
			i_loadTableValuesCallback(*luaState);

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
			luaState = NULL;
		}

		return !wereThereErrors;
	}
}