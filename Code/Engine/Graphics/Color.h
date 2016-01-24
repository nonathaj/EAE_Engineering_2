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
		Color(float r, float g, float b, float a = 1.0f) : red_(r), green_(g), blue_(b), alpha_(a) {}

		float r() const { return red_; }
		float g() const { return green_; }
		float b() const { return blue_; }
		float a() const { return alpha_; }

		void r(const float i_val) { red_ = i_val; }
		void g(const float i_val) { green_ = i_val; }
		void b(const float i_val) { blue_ = i_val; }
		void a(const float i_val) { alpha_ = i_val; }

		Color32 GetColor32() const;
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
		Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : red_(r), green_(g), blue_(b), alpha_(a) {}

		uint8_t r() const { return red_; }
		uint8_t g() const { return green_; }
		uint8_t b() const { return blue_; }
		uint8_t a() const { return alpha_; }

		void r(const uint8_t i_val) { red_ = i_val; }
		void g(const uint8_t i_val) { green_ = i_val; }
		void b(const uint8_t i_val) { blue_ = i_val; }
		void a(const uint8_t i_val) { alpha_ = i_val; }

		Color GetColor() const;
	private:
#if EAE6320_PLATFORM_D3D
		uint8_t blue_, green_, red_, alpha_;	// Direct3D expects the byte layout of a color to be different from what you might expect
#elif EAE6320_PLATFORM_GL
		uint8_t red_, green_, blue_, alpha_;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
#endif
	};
}

#endif //_LAME_GRAPHICS_COLOR_H
