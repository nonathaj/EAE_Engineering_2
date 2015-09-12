#ifndef _LAME_VERTEX_H
#define _LAME_VERTEX_H

#include <cstdint>

namespace Lame
{
	struct Vertex
	{
		// POSITION, 2 floats == 8 bytes, Offset = 0
		float x, y;
		// COLOR0, 4 uint8_ts == 4 bytes, Offset = 8
#if EAE6320_PLATFORM_D3D
		uint8_t b, g, r, a;	// Direct3D expects the byte layout of a color to be different from what you might expect
#elif EAE6320_PLATFORM_GL
		uint8_t r, g, b, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#endif
	};
}

#endif //_LAME_VERTEX_H
