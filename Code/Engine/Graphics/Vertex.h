#ifndef _LAME_VERTEX_H
#define _LAME_VERTEX_H

#include <cstdint>
#include "Color.h"
#include "../../External/Lua/Includes.h"

namespace Lame
{
	struct Vertex
	{
	public:
		// POSITION, 3 floats == 12 bytes, Offset = 0
		float x, y, z;

		// COLOR0, 4 uint8_ts == 4 bytes, Offset = 12
#if EAE6320_PLATFORM_D3D
		uint8_t b, g, r, a;	// Direct3D expects the byte layout of a color to be different from what you might expect
#elif EAE6320_PLATFORM_GL
		uint8_t r, g, b, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#endif
	};
}

#endif //_LAME_VERTEX_H
