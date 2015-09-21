
#include "Color.h"

namespace Lame
{
	Color::Color(float r, float g, float b, float a) 
		: red_(r), green_(g), blue_(b), alpha_(a) 
	{}

	Color32::Color32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) 
		: red_(r), green_(g), blue_(b), alpha_(a) 
	{}

	float Color::r() const { return red_; }
	float Color::g() const { return green_; }
	float Color::b() const { return blue_; }
	float Color::a() const { return alpha_; }

	uint8_t Color32::r() const { return red_; }
	uint8_t Color32::g() const { return green_; }
	uint8_t Color32::b() const { return blue_; }
	uint8_t Color32::a() const { return alpha_; }

	//Allow a Color to be converted to a Color32
	Color::operator Color32() const
	{
		return Color32(
			static_cast<uint8_t>(this->r() * 255.0f),
			static_cast<uint8_t>(this->g() * 255.0f),
			static_cast<uint8_t>(this->b() * 255.0f),
			static_cast<uint8_t>(this->a() * 255.0f)
		);
	}

	//Allow a Color32 to be converted to a Color
	Color32::operator Color() const
	{
		return Color(
			this->r() / 255.0f, 
			this->g() / 255.0f, 
			this->b() / 255.0f, 
			this->a() / 255.0f
		);
	}
}
