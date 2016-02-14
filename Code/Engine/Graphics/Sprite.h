#ifndef _LAME_SPRITE_H
#define _LAME_SPRITE_H

#include <memory>

#include "Effect.h"
#include "../Core/Color.h"
#include "../Core/Vertex.h"
#include "../Core/Rectangle2D.h"

namespace Lame
{
	class Texture;
	class RenderableMesh;

	class Sprite
	{
	public:
		static Sprite* Create(std::shared_ptr<Effect> i_effect, std::shared_ptr<Lame::Texture> i_texture, const Lame::Vector2& i_screen_pos_normalized, const float i_height_normalized, const Lame::Rectangle2D& i_texture_coords);
		static Sprite* Create(std::shared_ptr<Effect> i_effect, std::shared_ptr<Lame::Texture> i_texture, const Lame::Rectangle2D& i_screen_coords, const Lame::Rectangle2D& i_texture_coords);

		bool Render();

		inline std::shared_ptr<Effect> effect() const { return effect_; }
		inline std::shared_ptr<Texture> texture() const { return texture_; }
		inline std::shared_ptr<RenderableMesh> mesh() const { return mesh_; }

		inline Color color() const { return color_; }
		inline void color(const Color& i_color) { color_ = i_color; }

		bool screen_coords(const Lame::Rectangle2D& i_screen_coords);
		bool texture_coords(const Lame::Rectangle2D& i_texture_coords);
		Lame::Rectangle2D texture_coords() const;
		Lame::Rectangle2D screen_coords() const;

		bool SelectFromSheet(const size_t i_horz_count, const size_t i_vert_count, const size_t i_index);

	private:
		Sprite() : color_(Color::white) {}

		Lame::Vertex vertices[4];
		Color color_;

		Effect::ConstantHandle color_uniform_id;
		Effect::ConstantHandle texture_uniform_id;

		std::shared_ptr<Effect> effect_;
		std::shared_ptr<Texture> texture_;
		std::shared_ptr<RenderableMesh> mesh_;

		static char const * const ColorUniformName;
		static char const * const BaseTextureUniformName;
	};
}

#endif //_LAME_SPRITE_H
