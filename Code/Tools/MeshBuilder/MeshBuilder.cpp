// Header Files
//=============

#include "MeshBuilder.h"

#include <sstream>
#include <cassert>
#include <fstream>

#include "../../Engine/Windows/Functions.h"

#include "../../External/Lua/Includes.h"
#include "../../Engine/Core/Vertex.h"

#include "../../External/Lua/LuaHelper.h"

namespace
{
	bool LoadMesh(const std::string& i_source, std::vector<Lame::Vertex>& o_vertices, std::vector<uint32_t>& o_indices);

	template<typename CountType, typename VertexType, typename IndexType>
	bool WriteMeshBinary(const std::string& i_target, const std::vector<VertexType>& i_vertices, const std::vector<IndexType>& i_indices);
}

bool eae6320::MeshBuilder::Build( const std::vector<std::string>& i_arguments )
{
	std::vector<Lame::Vertex> vertices;
	std::vector<uint32_t> indices;

	if (!LoadMesh(m_path_source, vertices, indices))
		return false;

	//loaded indices are right-handed
#if EAE6320_PLATFORM_D3D
	//swap the index order to be left-handed
	size_t numTrianges = indices.size() / 3;
	for (size_t x = 0; x < numTrianges; x++)
		std::swap(indices[x * 3], indices[x * 3 + 2]);
#elif EAE6320_PLATFORM_GL
	//the indices are already in order.
#endif

	return WriteMeshBinary<uint32_t>(m_path_target, vertices, indices);
}

namespace
{
	bool LoadMesh(const std::string& i_source, std::vector<Lame::Vertex>& o_vertices, std::vector<uint32_t>& o_indices)
	{
		LuaHelper::LuaStack *stack = LuaHelper::LuaStack::Create(i_source);
		if (!stack)
		{
			eae6320::OutputErrorMessage("Failed to open the file and create lua state.", i_source.c_str());
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
					{
						std::vector<double> position;
						stack->Push("pos");
						if (stack->SwapTableKey() && stack->PeekArray(position) && position.size() == 3)
						{
							vert.position = Lame::Vector3(
								static_cast<float>(position[0]),
								static_cast<float>(position[1]),
								static_cast<float>(position[2]));
						}
						else
						{
							std::stringstream error;
							error << "Invalid position table in vertex " << x;
							eae6320::OutputErrorMessage(error.str().c_str(), i_source.c_str());
							delete stack;
							return false;
						}
						stack->Pop();
					}

					//vertex texture coordinates
					{
						std::vector<double> texcoords;
						stack->Push("texcoord");
						if (stack->SwapTableKey() && stack->PeekArray(texcoords) && texcoords.size() == 2)
						{
							vert.texcoord.x(static_cast<float>(texcoords[0]));
							vert.texcoord.y(1.0f - static_cast<float>(texcoords[1]));
						}
						else
						{
							std::stringstream error;
							error << "Invalid texcoord table in vertex " << x;
							eae6320::OutputErrorMessage(error.str().c_str(), i_source.c_str());
							delete stack;
							return false;
						}
						stack->Pop();
					}

					//vertex color
					{
						std::vector<double> color;
						stack->Push("color");
						if (stack->SwapTableKey() && stack->PeekArray(color) && color.size() == 4)
						{
							vert.color.r(static_cast<uint8_t>(color[0] * 255.0));
							vert.color.g(static_cast<uint8_t>(color[1] * 255.0));
							vert.color.b(static_cast<uint8_t>(color[2] * 255.0));
							vert.color.a(static_cast<uint8_t>(color[3] * 255.0));
						}
						else
						{
							std::stringstream error;
							error << "Invalid color table in vertex " << x;
							eae6320::OutputErrorMessage(error.str().c_str(), i_source.c_str());
							delete stack;
							return false;
						}
						stack->Pop();
					}
				}
				else
				{
					std::stringstream error;
					error << "Invalid vertex table for vertex " << x;
					eae6320::OutputErrorMessage(error.str().c_str(), i_source.c_str());
					delete stack;
					return false;
				}
				stack->Pop();

				o_vertices.push_back(vert);
			}
		}
		else
		{
			eae6320::OutputErrorMessage("Invalid Vertex Table", i_source.c_str());
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
					o_indices.push_back(static_cast<uint32_t>(triangleIndices[0]));
					o_indices.push_back(static_cast<uint32_t>(triangleIndices[1]));
					o_indices.push_back(static_cast<uint32_t>(triangleIndices[2]));
				}
				else
				{
					std::stringstream error;
					error << "Invalid triangle table for triangle " << x;
					eae6320::OutputErrorMessage(error.str().c_str(), i_source.c_str());
					delete stack;
					return false;
				}
				stack->Pop();
			}
		}
		else
		{
			eae6320::OutputErrorMessage("Invalid Triangle Index Table", i_source.c_str());
			delete stack;
			return false;
		}
		stack->Pop();

		delete stack;
		return true;
	}

	template<typename CountType, typename VertexType, typename IndexType>
	bool WriteMeshBinary(const std::string& i_target, const std::vector<VertexType>& i_vertices, const std::vector<IndexType>& i_indices)
	{
		CountType vertexCount32 = static_cast<CountType>(i_vertices.size());
		CountType indexCount32 = static_cast<CountType>(i_indices.size());
		std::ofstream out(i_target, std::ofstream::binary);
		if (!out)
		{
			eae6320::OutputErrorMessage("Failed to open the output file for writing", i_target.c_str());
			return false;
		}

		//write the data
		out.write(reinterpret_cast<char*>(&vertexCount32), sizeof(vertexCount32));
		out.write(reinterpret_cast<char*>(&indexCount32), sizeof(indexCount32));
		out.write(reinterpret_cast<const char*>(i_vertices.data()), sizeof(*i_vertices.data()) * i_vertices.size());
		out.write(reinterpret_cast<const char*>(i_indices.data()), sizeof(*i_indices.data()) * i_indices.size());

		out.close();
		return true;
	}
}
