#ifndef _LAME_FONTRENDERER_H
#define _LAME_FONTRENDERER_H

#include <memory>
#include <string>

#include "Color.h"

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Lame
{
	class Context;
	class Rectangle2D;
	class Vector2;

	namespace Font
	{
		enum class Pitch { Default, Monospace, };
		enum class Type { Normal, Bold, Italics, BoldItalics };
		enum class HorizontalAlignment { Left, Center, Right, };
	}

	class FontRenderer
	{
	public:
		static FontRenderer* Create(std::shared_ptr<Lame::Context> i_context, const Vector2& i_screen_size, Font::Pitch i_pitch, Font::Type i_type, const char* i_font = "");
		~FontRenderer();

		bool Render(const char* i_str, const Rectangle2D& i_screen_rect, Font::HorizontalAlignment  i_align = Font::HorizontalAlignment::Left, bool i_word_wrap = false, const Color32& i_color = Color32::white) const;

		std::shared_ptr<Lame::Context> context() const { return context_; }
	private:
		FontRenderer() { }

		std::shared_ptr<Lame::Context> context_;

#if EAE6320_PLATFORM_D3D
		//hack to get around including d3d headers here.  Required internal casts
		void* font;
#endif
	};
}

#endif //_LAME_FONTRENDERER_H
