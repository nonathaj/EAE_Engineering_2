
#include "EffectBuilder.h"

#include <sstream>
#include <cassert>
#include <fstream>

#include "../BuilderHelper/UtilityFunctions.h"
#include "../../Engine/Windows/Functions.h"
#include "../../Engine/Graphics/Effect.h"

#include "../../External/Lua/LuaHelper.h"

bool EffectBuilder::Build(const std::vector<std::string>&)
{
	////////////////////////////////////////////
	//Data we need
	////////////////////////////////////////////
	std::string vertex, fragment;
	Engine::EnumMask<Lame::RenderState> renderMask;

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
		std::unordered_map<std::string, std::string> strs;
		std::unordered_map<std::string, bool> flags;
		bool dataReadInSuccessfully = stack->PeekDictionary(strs) && stack->PeekDictionary(flags);
		delete stack;

		if (!dataReadInSuccessfully)
		{
			eae6320::OutputErrorMessage("Failed to read lua data from file.", m_path_source);
			delete[] stack;
			return false;
		}

		vertex = strs["vertex"];
		fragment = strs["fragment"];
		renderMask.set(Lame::RenderState::Transparency, flags["transparency"]);
		renderMask.set(Lame::RenderState::DepthTest, flags["depth_test"]);
		renderMask.set(Lame::RenderState::DepthWrite, flags["depth_write"]);
		renderMask.set(Lame::RenderState::FaceCull, flags["face_cull"]);
		renderMask.set(Lame::RenderState::Wireframe, flags["wireframe"]);
	}

	////////////////////////////////////////////
	//Fix up loaded data
	////////////////////////////////////////////
	{
		//Add the relative folder location of these built assets
		std::string relativeFolder, outError;
		if (!eae6320::GetEnvironmentVariableA("GameDataDir", relativeFolder, &outError))
		{
			std::stringstream error;
			error << "Failed to load GameDataDir environment variable. " << outError;
			eae6320::OutputErrorMessage(error.str().c_str());
			return false;
		}
		vertex = relativeFolder + vertex;
		fragment = relativeFolder + fragment;
	}

	////////////////////////////////////////////
	//Write data to binary
	////////////////////////////////////////////
	{
		//find the length of the vertex string
		uint8_t vertexStringLength = static_cast<uint8_t>(vertex.length());

		//warn about vertex string length being too long
		if (vertex.length() > std::numeric_limits<uint8_t>::max())
		{
			std::stringstream error;
			error << "Length of vertex path for effect " << m_path_source << " is greater than "
				<< sizeof(vertexStringLength) << " (actual value=" << vertex.length() << ")";
			eae6320::OutputErrorMessage(error.str().c_str(), m_path_source);
			return false;
		}

		std::ofstream out(m_path_target, std::ofstream::binary);
		if(!out)
		{
			eae6320::OutputErrorMessage("Failed to open the output file for writing", m_path_target);
			return false;
		}

		//write the data
		out.write(reinterpret_cast<char*>(&renderMask), sizeof(renderMask));
		out.write(reinterpret_cast<char*>(&vertexStringLength), sizeof(vertexStringLength));
		out.write(vertex.c_str(), vertexStringLength + 1);
		out.write(fragment.c_str(), fragment.length() + 1);

		out.close();
	}

	return true;
}
