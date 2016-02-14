#ifndef _LAME_VERTEX_H
#define _LAME_VERTEX_H

#include <cstdint>
#include <memory>
#include "../Core/Color.h"
#include "../Core/Vector2.h"
#include "../Core/Vector3.h"

namespace Lame
{
	class Context;

	struct Vertex
	{
	public:
		Lame::Vector3 position;		// POSITION, 3 floats == 12 bytes, Offset = 0
		Lame::Vector2 texcoord;		// TEXCOORD0, 2 floats = 8 bytes, Offset 12
		Color32 color;				// COLOR0, 4 uint8_ts == 4 bytes, Offset = 20

		Vertex() {}
		Vertex(Lame::Vector3 i_pos, Lame::Vector2 i_tex, Color32 i_color) : position(i_pos), texcoord(i_tex), color(i_color) {}
	};
}

#endif //_LAME_VERTEX_H
