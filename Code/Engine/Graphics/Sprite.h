#ifndef _LAME_SPRITE_H
#define _LAME_SPRITE_H

#include <memory>

#include "Effect.h"
#include "Color.h"
#include "../Core/Rectangle2D.h"

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Lame
{
	class Texture;

	class Sprite
	{
	public:
		static Sprite* Create(std::shared_ptr<Effect> i_effect, std::shared_ptr<Lame::Texture> i_texture, const Engine::Rectangle2D& i_screen_pos);

		bool Render();

		inline std::shared_ptr<Effect> effect() const { return effect_; }
		inline std::shared_ptr<Texture> texture() const { return texture_; }
		inline Color color() const { return color_; }
		inline void color(const Color& i_color) { color_ = i_color; }

		inline Engine::Rectangle2D texture_coord() const { return texture_coord_; }
		inline Engine::Rectangle2D screen_pos() const { return screen_pos_; }

		inline void texture_coord(const Engine::Rectangle2D& i_tex);
		inline void screen_pos(const Engine::Rectangle2D& i_screen_pos);
	private:
		Sprite() : color_(Color::white) {}

		bool UpdateVertexData();

		Color color_;
		Engine::Rectangle2D texture_coord_;
		Engine::Rectangle2D screen_pos_;

		std::shared_ptr<Effect> effect_;
		std::shared_ptr<Texture> texture_;

#if EAE6320_PLATFORM_D3D
		IDirect3DVertexBuffer9 *vertex_buffer_;
		IDirect3DVertexDeclaration9 *vertex_declaration_;
#elif EAE6320_PLATFORM_GL
		GLuint vertex_array_id_;
#endif
		Effect::ConstantHandle color_uniform_id;
		Effect::ConstantHandle texture_uniform_id;

		static char const * const ColorUniformName;
		static char const * const BaseTextureUniformName;
	};
}

#endif //_LAME_SPRITE_H