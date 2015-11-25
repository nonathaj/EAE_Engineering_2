// Header Files
//=============

#include "MeshBuilder.h"

#include <sstream>
#include <cassert>
#include <fstream>

#include "../../Engine/Windows/Functions.h"

#include "../../External/Lua/Includes.h"
#include "../../Engine/Graphics/Vertex.h"

#include "../../External/Lua/LuaHelper.h"

bool eae6320::MeshBuilder::Build( const std::vector<std::string>& )
{
	////////////////////////////////////////////
	//Data we need
	////////////////////////////////////////////
	std::vector<Lame::Vertex> vertices;
	std::vector<uint32_t> indices;

	////////////////////////////////////////////
	//Load data from Lua
	////////////////////////////////////////////
	{
		LuaHelper::LuaStack *stack = LuaHelper::LuaStack::Create(m_path_source);
		if (!stack)
		{
			eae6320::OutputErrorMessage("Failed to open the file and create lua state.", m_path_source);
			return false;
		}

		//vertices list table
		stack->Push("vertex");
		if (stack->SwapTableKey() && stack->IsTable())
		{
			size_t vertexCount = stack->TableLength();
			for (size_t x = 0; x < vertexCount; x++)
			{
				Lame::Vertex vert;
				//a single vertex's table
				stack->Push(static_cast<lua_Unsigned>(x + 1));
				if (stack->SwapTableKey() && stack->IsTable())
				{
					//vertex position
					std::vector<double> position;
					stack->Push("pos");
					if (stack->SwapTableKey() && stack->PeekArray(position) && position.size() == 3)
					{
						vert.x = static_cast<float>(position[0]);
						vert.y = static_cast<float>(position[1]);
						vert.z = static_cast<float>(position[2]);
					}
					else
					{
						std::stringstream error;
						error << "Invalid position table in vertex " << x;
						eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
						delete stack;
						return false;
					}
					stack->Pop();

					//vertex color
					std::vector<double> color;
					stack->Push("color");
					if (stack->SwapTableKey() && stack->PeekArray(color) && color.size() == 4)
					{
						vert.r = static_cast<uint8_t>(color[0] * 255.0);
						vert.g = static_cast<uint8_t>(color[1] * 255.0);
						vert.b = static_cast<uint8_t>(color[2] * 255.0);
						vert.a = static_cast<uint8_t>(color[3] * 255.0);
					}
					else
					{
						std::stringstream error;
						error << "Invalid color table in vertex " << x;
						eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
						delete stack;
						return false;
					}
					stack->Pop();
				}
				else
				{
					std::stringstream error;
					error << "Invalid vertex table for vertex " << x;
					eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
					delete stack;
					return false;
				}
				stack->Pop();

				vertices.push_back(vert);
			}
		}
		else
		{
			eae6320::OutputErrorMessage("Invalid Vertex Table", m_path_source);
			delete stack;
			return false;
		}
		stack->Pop();

		//triangle table
		stack->Push("index");
		if (stack->SwapTableKey() && stack->IsTable())
		{
			size_t triangleCount = stack->TableLength();
			size_t indexCount = triangleCount * 3;
			for (size_t x = 0; x < triangleCount; x++)
			{
				//triangle
				std::vector<lua_Integer> triangleIndices;
				stack->Push(static_cast<lua_Unsigned>(x + 1));
				if (stack->SwapTableKey() && stack->PeekArray(triangleIndices) && triangleIndices.size() == 3)
				{
					indices.push_back(static_cast<uint32_t>(triangleIndices[0]));
					indices.push_back(static_cast<uint32_t>(triangleIndices[1]));
					indices.push_back(static_cast<uint32_t>(triangleIndices[2]));
				}
				else
				{
					std::stringstream error;
					error << "Invalid triangle table for triangle " << x;
					eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
					delete stack;
					return false;
				}
				stack->Pop();
			}
		} 
		else
		{
			eae6320::OutputErrorMessage("Invalid Triangle Index Table", m_path_source);
			delete stack;
			return false;
		}
		stack->Pop();

		delete stack;
	}

	////////////////////////////////////////////
	//Fix up loaded data
	////////////////////////////////////////////
	{
		//loaded indices are right-handed
#if EAE6320_PLATFORM_D3D
		//swap the index order to be left-handed
		size_t numTrianges = indices.size() / 3;
		for (size_t x = 0; x < numTrianges; x++)
			std::swap(indices[x * 3], indices[x * 3 + 2]);
#elif EAE6320_PLATFORM_GL
		//the indices are already in order.
#endif
	}


	////////////////////////////////////////////
	//Write data to binary
	////////////////////////////////////////////
	{
		uint32_t vertexCount32 = static_cast<uint32_t>(vertices.size());
		uint32_t indexCount32 = static_cast<uint32_t>(indices.size());
		std::ofstream out(m_path_target, std::ofstream::binary);
		if(!out)
		{
			eae6320::OutputErrorMessage("Failed to open the output file for writing", m_path_target);
			return false;
		}

		//write the data
		out.write(reinterpret_cast<char*>(&vertexCount32), sizeof(vertexCount32));
		out.write(reinterpret_cast<char*>(&indexCount32), sizeof(indexCount32));
		out.write(reinterpret_cast<char*>(vertices.data()), sizeof(Lame::Vertex) * vertices.size());
		out.write(reinterpret_cast<char*>(indices.data()), sizeof(uint32_t) * indices.size());

		out.close();
	}

	return true;
}
