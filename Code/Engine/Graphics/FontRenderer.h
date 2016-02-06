#ifndef _LAME_FONTRENDERER_H
#define _LAME_FONTRENDERER_H

#include <memory>
#include <string>

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#include <D3dx9core.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Lame
{
	class Context;
	class Rectangle2D;
	class Vector2;
	class Color32;

	namespace Font
	{
		enum class Pitch { Default, Monospace, };
		enum class Type { Normal, Bold, Italics, BoldItalics };
	}

	class FontRenderer
	{
	public:
		static FontRenderer* Create(std::shared_ptr<Lame::Context> i_context, const Vector2& i_screen_size, Font::Pitch i_pitch, Font::Type i_type);
		~FontRenderer();

		bool Render(const std::string& i_string, const Rectangle2D& i_screen_rect, const Color32& i_color);
	private:
		FontRenderer() { }

#if EAE6320_PLATFORM_D3D
		LPD3DXFONT font;
#endif
	};
}

#endif //_LAME_FONTRENDERER_H
