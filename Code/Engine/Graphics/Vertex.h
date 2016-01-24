#ifndef _LAME_VERTEX_H
#define _LAME_VERTEX_H

#include <cstdint>
#include "Color.h"
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
		float u, v;

		// COLOR0, 4 uint8_ts == 4 bytes, Offset = 20
		Color32 color;
	};
}

#endif //_LAME_VERTEX_H
