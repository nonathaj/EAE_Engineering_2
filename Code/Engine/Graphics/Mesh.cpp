
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
	bool LoadMeshAssetTableFromLua(const std::string &i_path, Lame::Vertex*& o_vertices, size_t& o_vertex_count, uint32_t*& o_indices, size_t& o_index_count);
	bool LoadMesh(lua_State* io_luaStateFrom, Lame::Vertex*& o_vertices, size_t& o_vertex_count, uint32_t*& o_indices, size_t& o_index_count);
	bool LoadColor(lua_State* io_luaStateFrom, double& o_r, double& o_g, double& o_b, double& o_a);
	bool LoadPosition(lua_State* io_luaStateFrom, double& o_x, double& o_y);
	bool LoadVertex(lua_State* io_luaStateFrom, Lame::Vertex& o_vertex);

	//Returns a double from a table at the top of the stack
	double GetDoubleFromTable(lua_State& io_luaStateFrom, size_t i_indexInTable);
}

namespace Lame
{
	Mesh* Mesh::Create(std::string i_mesh_path)
	{
		Vertex *vertices;
		size_t vertex_count;

		uint32_t *indices;
		size_t index_count;

		if (LoadMeshAssetTableFromLua(i_mesh_path, vertices, vertex_count, indices, index_count))
		{
			Mesh *mesh = Create(vertices, vertex_count, indices, index_count);
			delete[] vertices;
			delete[] indices;
			return mesh;
		}
		else
		{
			DEBUG_PRINT("Failed to load mesh");
			return nullptr;
		}
	}
}

namespace
{
	bool LoadMesh(lua_State* io_luaStateFrom, Lame::Vertex*& o_vertices, size_t& o_vertex_count, uint32_t*& o_indices, size_t& o_index_count)
	{
		//Load the vertex data
		{
			char const * const key = "vertex";

			//push the vertex array
			lua_pushstring(io_luaStateFrom, key);
			lua_gettable(io_luaStateFrom, -2);
			if (!lua_istable(io_luaStateFrom, -1))
			{
				DEBUG_PRINT("The value at \"%s\" must be a table instead of a %s", key, luaL_typename(io_luaStateFrom, -1));
				lua_pop(io_luaStateFrom, 1);
				return false;
			}

			//handle the vertex array
			o_vertex_count = luaL_len(io_luaStateFrom, -1);
			o_vertices = new Lame::Vertex[o_vertex_count];
			bool vertex_failed;
			for (size_t x = 1; x <= o_vertex_count; x++)
			{
				lua_pushinteger(io_luaStateFrom, x);	//the key we are looking for
				lua_gettable(io_luaStateFrom, -2);		//check the table for the key, and replace the key with the value
				vertex_failed = !LoadVertex(io_luaStateFrom, o_vertices[x-1]);
				lua_pop(io_luaStateFrom, 1);
				if (vertex_failed)
				{
					DEBUG_PRINT("The vertex %d must be a table instead of a %s", x, luaL_typename(io_luaStateFrom, -1));
					delete[] o_vertices;
					return false;
				}
			}

			//pop the vertex array off the stack
			lua_pop(io_luaStateFrom, 1);
		}

		//Load the index data
		{
			char const * const key = "index";

			//push the triangle index array
			lua_pushstring(io_luaStateFrom, key);
			lua_gettable(io_luaStateFrom, -2);
			if (!lua_istable(io_luaStateFrom, -1))
			{
				DEBUG_PRINT("The value at \"%s\" must be a table instead of a %s", key, luaL_typename(io_luaStateFrom, -1));
				lua_pop(io_luaStateFrom, 1);
				delete[] o_vertices;
				return false;
			}

			//handle the triangle index array
			int triangle_count = luaL_len(io_luaStateFrom, -1);
			o_index_count = triangle_count * 3;
			o_indices = new uint32_t[o_index_count];
			for (int x = 1; x <= triangle_count; x++)
			{
				//push the triangle
				lua_pushinteger(io_luaStateFrom, x);	//the key we are looking for
				lua_gettable(io_luaStateFrom, -2);		//check the table for the key, and replace the key with the value

				//handle the triangle
				if (lua_istable(io_luaStateFrom, -1))
				{
					int triangleStartIndex = (x - 1) * 3;
					//1
					lua_pushinteger(io_luaStateFrom, 1);
					lua_gettable(io_luaStateFrom, -2);
					o_indices[triangleStartIndex] = static_cast<uint32_t>(lua_tonumber(io_luaStateFrom, -1));
					lua_pop(io_luaStateFrom, 1);

					//2
					lua_pushinteger(io_luaStateFrom, 2);
					lua_gettable(io_luaStateFrom, -2);
					o_indices[triangleStartIndex + 1] = static_cast<uint32_t>(lua_tonumber(io_luaStateFrom, -1));
					lua_pop(io_luaStateFrom, 1);
					
					//3
					lua_pushinteger(io_luaStateFrom, 3);
					lua_gettable(io_luaStateFrom, -2);
					o_indices[triangleStartIndex + 2] = static_cast<uint32_t>(lua_tonumber(io_luaStateFrom, -1));
					lua_pop(io_luaStateFrom, 1);
				}
				else
				{
					DEBUG_PRINT("The vertex %d must be a table instead of a %s", x, luaL_typename(io_luaStateFrom, -1));
					lua_pop(io_luaStateFrom, 1);
					delete[] o_vertices;
					delete[] o_indices;
					return false;
				}

				//pop the triangle table
				lua_pop(io_luaStateFrom, 1);
			}

			//Pop the index array
			lua_pop(io_luaStateFrom, 1);
		}

		return true;
	}

	bool LoadMeshAssetTableFromLua(const std::string &i_path, Lame::Vertex*& o_vertices, size_t& o_vertex_count, uint32_t*& o_indices, size_t& o_index_count) 
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
		if (!LoadMesh(luaState, o_vertices, o_vertex_count, o_indices, o_index_count))
		{
			DEBUG_PRINT("Failed to load the mesh");
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

	bool LoadVertex(lua_State* io_luaStateFrom, Lame::Vertex& o_vertex)
	{
		//Ensure we have a valid table with the vertex data
		if (lua_istable(io_luaStateFrom, -1))
		{
			//load the position
			lua_pushstring(io_luaStateFrom, "pos");
			lua_gettable(io_luaStateFrom, -2);
			double x, y;
			if (!LoadPosition(io_luaStateFrom, x, y))
			{
				DEBUG_PRINT("Invalid vertex position data");
				return false;
			}
			lua_pop(io_luaStateFrom, 1);
			o_vertex.x = static_cast<float>(x);
			o_vertex.y = static_cast<float>(y);

			//Load the color
			double r, g, b, a;
			lua_pushstring(io_luaStateFrom, "color");
			lua_gettable(io_luaStateFrom, -2);
			if (!LoadColor(io_luaStateFrom, r, g, b, a))
			{
				DEBUG_PRINT("Invalid vertex color data");
				return false;
			}
			lua_pop(io_luaStateFrom, 1);

			o_vertex.r = static_cast<uint8_t>(r * 255.0);
			o_vertex.g = static_cast<uint8_t>(g * 255.0);
			o_vertex.b = static_cast<uint8_t>(b * 255.0);
			o_vertex.a = static_cast<uint8_t>(a * 255.0);

			return true;
		}
		else
		{
			DEBUG_PRINT("Vertex attempting to load is not a table");
			return false;
		}
	}

	//Returns a double from a table at the top of the stack
	double GetDoubleFromTable(lua_State* io_luaStateFrom, size_t i_indexInTable)
	{
		double value = std::numeric_limits<double>::quiet_NaN();
		
		if (lua_istable(io_luaStateFrom, -1))
		{
			lua_pushinteger(io_luaStateFrom, i_indexInTable);
			lua_gettable(io_luaStateFrom, -2);
			if (lua_isnumber(io_luaStateFrom, -1))
				value = lua_tonumber(io_luaStateFrom, -1);
			lua_pop(io_luaStateFrom, 1);
		}

		return value;
	}

	bool LoadPosition(lua_State* io_luaStateFrom, double& o_x, double& o_y)
	{
		if (!lua_istable(io_luaStateFrom, -1))
		{
			DEBUG_PRINT("Position attempting to load is not a table");
			return false;
		}
		
		o_x = GetDoubleFromTable(io_luaStateFrom, 1);
		o_y = GetDoubleFromTable(io_luaStateFrom, 2);

		if (std::isnan(o_x) || std::isnan(o_y))
		{
			DEBUG_PRINT("Invalid numbers in position (%f, %f)", o_x, o_y);
			return false;
		}
		else
		{
			return true;
		}
	}

	bool LoadColor(lua_State* io_luaStateFrom, double& o_r, double& o_g, double& o_b, double& o_a)
	{
		if (!lua_istable(io_luaStateFrom, -1))
		{
			DEBUG_PRINT("Color attempting to load is not a table");
			return false;
		}

		o_r = GetDoubleFromTable(io_luaStateFrom, 1);
		o_g = GetDoubleFromTable(io_luaStateFrom, 2);
		o_b = GetDoubleFromTable(io_luaStateFrom, 3);
		o_a = GetDoubleFromTable(io_luaStateFrom, 4);

		if (std::isnan(o_r) || std::isnan(o_g) || std::isnan(o_b) || std::isnan(o_a))
		{
			DEBUG_PRINT("Invalid numbers in color (%f, %f, %f, %f)", o_r, o_g, o_b, o_a);
			return false;
		}
		else
		{
			return true;
		}
	}
}
