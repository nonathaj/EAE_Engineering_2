
#include "Sprite.h"

namespace Lame
{
	char const * const Sprite::ColorUniformName = "color";
	char const * const Sprite::BaseTextureUniformName = "base_texture";


	bool Sprite::texture_coord(const Engine::Rectangle2D& i_tex)
	{
		return UpdateVertexData();
	}

	bool Sprite::screen_pos(const Engine::Vector2& i_screen_pos_normalized)
	{
		return UpdateVertexData();
	}


	inline Engine::Rectangle2D Sprite::texture_coord() const
	{
		return Engine::Rectangle2D();
	}

	inline Engine::Rectangle2D Sprite::screen_pos() const
	{
		return Engine::Rectangle2D();
	}
}
