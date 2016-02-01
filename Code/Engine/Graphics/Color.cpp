
#include "Color.h"

namespace Lame
{
	const Color Color::clear(0.0f, 0.0f, 0.0f, 0.0f);
	const Color Color::white(1.0f, 1.0f, 1.0f, 1.0f);
	const Color Color::black(0.0f, 0.0f, 0.0f, 1.0f);
	const Color Color::red(1.0f, 0.0f, 0.0f, 1.0f);
	const Color Color::green(0.0f, 1.0f, 0.0f, 1.0f);
	const Color Color::blue(0.0f, 0.0f, 1.0f, 1.0f);

	const Color32 Color32::clear(0, 0, 0, 0);
	const Color32 Color32::white(255, 255, 255, 255);
	const Color32 Color32::black(0, 0, 0, 255);
	const Color32 Color32::red(255, 0, 0, 255);
	const Color32 Color32::green(0, 255, 0, 255);
	const Color32 Color32::blue(0, 0, 255, 255);

	//Allow a Color32 to be converted to a Color
	Color::Color(const Color32& i_color) :
		red_(i_color.r() / 255.0f),
		green_(i_color.g() / 255.0f),
		blue_(i_color.b() / 255.0f),
		alpha_(i_color.a() / 255.0f)
	{
	}

	//Allow a Color to be converted to a Color32
	Color32::Color32(const Color& i_color) :
		red_(static_cast<uint8_t>(i_color.r() * 255.0f)),
		green_(static_cast<uint8_t>(i_color.g() * 255.0f)),
		blue_(static_cast<uint8_t>(i_color.b() * 255.0f)),
		alpha_(static_cast<uint8_t>(i_color.a() * 255.0f))
	{
	}
}
