#ifndef _LAME_VERTEX_H
#define _LAME_VERTEX_H

#include <cstdint>
#include "Color.h"
#include "../Core/Vector2.h"
#include "../Core/Vector3.h"
#include "../../External/Lua/Includes.h"

namespace Lame
{
	struct Vertex
	{
	public:
		// POSITION, 3 floats == 12 bytes, Offset = 0
		Engine::Vector3 position;

		// TEXCOORD0, 2 floats = 8 bytes, Offset 12
		Engine::Vector2 texcoord;

		// COLOR0, 4 uint8_ts == 4 bytes, Offset = 20
		Color32 color;

		Vertex() {}
		Vertex(Engine::Vector3 i_pos, Engine::Vector2 i_tex, Color32 i_color) : position(i_pos), texcoord(i_tex), color(i_color) {}
	};
}

#endif //_LAME_VERTEX_H
