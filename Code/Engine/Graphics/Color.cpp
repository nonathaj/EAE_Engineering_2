
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

	//Allow a Color to be converted to a Color32
	Color32 Color::GetColor32() const
	{
		const float color32Max = 255.0f;
		return Color32(
			static_cast<uint8_t>(this->r() * color32Max),
			static_cast<uint8_t>(this->g() * color32Max),
			static_cast<uint8_t>(this->b() * color32Max),
			static_cast<uint8_t>(this->a() * color32Max) );
	}

	//Allow a Color32 to be converted to a Color
	Color Color32::GetColor() const
	{
		const float color32Max = 255.0f;
		return Color(
			this->r() / color32Max, 
			this->g() / color32Max, 
			this->b() / color32Max, 
			this->a() / color32Max );
	}
}
