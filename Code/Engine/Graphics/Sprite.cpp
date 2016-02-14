
#include "Sprite.h"

#include "../Core/Vertex.h"
#include "Context.h"
#include "Texture.h"
#include "Effect.h"
#include "RenderableMesh.h"
#include "Graphics.h"
#include "../Core/Mesh.h"
#include "../System/UserOutput.h"
#include "../System/Console.h"

namespace Lame
{
	char const * const Sprite::ColorUniformName = "color";
	char const * const Sprite::BaseTextureUniformName = "base_texture";

	Sprite* Sprite::Create(std::shared_ptr<Effect> i_effect, std::shared_ptr<Lame::Texture> i_texture, const Lame::Vector2& i_screen_pos_normalized, const float i_height_normalized, const Lame::Rectangle2D& i_texture_coords)
	{
		if (!i_effect || !i_texture)
			return nullptr;

		const float texture_aspect = static_cast<float>(i_texture->width()) / static_cast<float>(i_texture->height());
		const float aspect = texture_aspect * i_effect->get_context()->aspect_ratio();
		const float width_normalized = i_height_normalized / aspect;
		const Lame::Rectangle2D screen_coords(i_screen_pos_normalized, Lame::Vector2(width_normalized, i_height_normalized));
		return Create(i_effect, i_texture, screen_coords, i_texture_coords);
	}

	Sprite* Sprite::Create(std::shared_ptr<Effect> i_effect, std::shared_ptr<Lame::Texture> i_texture, const Lame::Rectangle2D& i_screen_coords, const Lame::Rectangle2D& i_texture_coords)
	{
		if (!i_effect || !i_texture)
			return nullptr;
		
		Effect::ConstantHandle color_uniform;
		Effect::ConstantHandle texture_uniform;
		if (!i_effect->CacheConstant(Lame::Effect::Fragment, ColorUniformName, color_uniform) ||
			!i_effect->CacheConstant(Lame::Effect::Fragment, BaseTextureUniformName, texture_uniform))
		{
			Lame::UserOutput::Display("Unable to cache constants in Effect for Sprite");
			return nullptr;
		}

		Sprite* sprite = new Sprite();
		if (!sprite)
		{
			Lame::UserOutput::Display("Insufficient memory to create Sprite");
			return nullptr;
		}

		std::shared_ptr<RenderableMesh> mesh;
		{
			const Color32 color = Color32::white;
			const Lame::Rectangle2D realsc = Context::GetRealScreenCoord(i_screen_coords);
			sprite->vertices[0] = Vertex(Lame::Vector2(realsc.left(), realsc.top()), Lame::Vector2(i_texture_coords.left(), i_texture_coords.top()), color);
			sprite->vertices[1] = Vertex(Lame::Vector2(realsc.right(), realsc.top()), Lame::Vector2(i_texture_coords.right(), i_texture_coords.top()), color);
			sprite->vertices[2] = Vertex(Lame::Vector2(realsc.left(), realsc.bottom()), Lame::Vector2(i_texture_coords.left(), i_texture_coords.bottom()), color);
			sprite->vertices[3] = Vertex(Lame::Vector2(realsc.right(), realsc.bottom()), Lame::Vector2(i_texture_coords.right(), i_texture_coords.bottom()), color);

			mesh = std::shared_ptr<RenderableMesh>(RenderableMesh::CreateEmpty(false, i_effect->get_context(), Lame::Mesh::PrimitiveType::TriangleStrip, 4, 0));
			if (!mesh || !mesh->UpdateVertices(sprite->vertices))
			{
				Lame::UserOutput::Display("Unable to create RenderableMesh for Sprite");
				delete sprite;
				return nullptr;
			}
		}

		sprite->effect_ = i_effect;
		sprite->texture_ = i_texture;
		sprite->mesh_ = mesh;
		sprite->color_uniform_id = color_uniform;
		sprite->texture_uniform_id = texture_uniform;
		return sprite;
	}

	bool Sprite::Render()
	{
		return effect()->Bind() &&
			effect()->SetConstant(Lame::Effect::Shader::Fragment, color_uniform_id, color()) &&
			effect()->SetConstant(Lame::Effect::Shader::Fragment, texture_uniform_id, texture().get()) &&
			mesh()->Draw();
	}

	bool Sprite::screen_coords(const Lame::Rectangle2D& i_screen_coords)
	{
		Lame::Rectangle2D realsc = Context::GetRealScreenCoord(i_screen_coords);
		vertices[0].position = Lame::Vector2(realsc.left(), realsc.top());
		vertices[1].position = Lame::Vector2(realsc.right(), realsc.top());
		vertices[2].position = Lame::Vector2(realsc.left(), realsc.bottom());
		vertices[3].position = Lame::Vector2(realsc.right(), realsc.bottom());
		return mesh()->UpdateVertices(vertices);
	}

	bool Sprite::texture_coords(const Lame::Rectangle2D& i_texture_coords)
	{
		vertices[0].texcoord = Lame::Vector2(i_texture_coords.left(), i_texture_coords.top());
		vertices[1].texcoord = Lame::Vector2(i_texture_coords.right(), i_texture_coords.top());
		vertices[2].texcoord = Lame::Vector2(i_texture_coords.left(), i_texture_coords.bottom());
		vertices[3].texcoord = Lame::Vector2(i_texture_coords.right(), i_texture_coords.bottom());
		return mesh()->UpdateVertices(vertices);
	}

	Lame::Rectangle2D Sprite::texture_coords() const
	{
		return Lame::Rectangle2D(
			vertices[0].texcoord.x(), vertices[3].texcoord.x(), vertices[0].texcoord.y(), vertices[3].texcoord.y());
	}

	Lame::Rectangle2D Sprite::screen_coords() const
	{
		Lame::Rectangle2D real_screen(vertices[0].position.x(), vertices[3].position.x(), vertices[0].position.y(), vertices[3].position.y());
		return Context::GetVirtualScreenCoord(real_screen);
	}

	bool Sprite::SelectFromSheet(const size_t i_horz_count, const size_t i_vert_count, const size_t i_index)
	{
		if (i_horz_count == 0 || i_vert_count == 0)
			return false;

		const float width = 1.0f / static_cast<float>(i_horz_count);
		const float height = 1.0f / static_cast<float>(i_vert_count);
		const size_t x = i_index % i_horz_count;
		const size_t y = i_index / i_horz_count;
		if (y >= i_vert_count)
			return false;
		Lame::Rectangle2D tex(x * width, (x + 1) * width, y * height, (y + 1) * height);
		return texture_coords(tex);
	}
}
