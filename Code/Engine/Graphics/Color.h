#ifndef _LAME_GRAPHICS_COLOR_H
#define _LAME_GRAPHICS_COLOR_H

#include <cstdint>

namespace Lame
{
	struct Color32;

	struct Color
	{
	public:
		Color() { }
		Color(float r, float g, float b, float a = 1.0f);

		float r() const;
		float g() const;
		float b() const;
		float a() const;

		operator Color32() const;
	private:
#if EAE6320_PLATFORM_D3D
		float blue_, green_, red_, alpha_;
#elif EAE6320_PLATFORM_GL
		float red_, green_, blue_, alpha_;
#endif
	};

	struct Color32
	{
	public:
		Color32() { }
		Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

		uint8_t r() const;
		uint8_t g() const;
		uint8_t b() const;
		uint8_t a() const;

		operator Color() const;
	private:
#if EAE6320_PLATFORM_D3D
		uint8_t blue_, green_, red_, alpha_;	// Direct3D expects the byte layout of a color to be different from what you might expect
#elif EAE6320_PLATFORM_GL
		uint8_t red_, green_, blue_, alpha_;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#endif
	};
}

#endif //_LAME_GRAPHICS_COLOR_H
