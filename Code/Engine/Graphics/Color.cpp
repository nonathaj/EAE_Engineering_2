
#include "Color.h"

namespace Lame
{
	//Allow a Color to be converted to a Color32
	Color32 Color::GetColor32() const
	{
		return Color32(
			static_cast<uint8_t>(this->r() * 255.0f),
			static_cast<uint8_t>(this->g() * 255.0f),
			static_cast<uint8_t>(this->b() * 255.0f),
			static_cast<uint8_t>(this->a() * 255.0f)
		);
	}

	//Allow a Color32 to be converted to a Color
	Color Color32::GetColor() const
	{
		return Color(
			this->r() / 255.0f, 
			this->g() / 255.0f, 
			this->b() / 255.0f, 
			this->a() / 255.0f
		);
	}
}
