
#include "../FontRenderer.h"

#include <D3dx9core.h>

#include "../Context.h"
#include "../../Core/Rectangle2D.h"
#include "../../Core/Vector2.h"

#include "../../../Engine/System/Console.h"

namespace Lame
{
	FontRenderer* FontRenderer::Create(std::shared_ptr<Lame::Context> i_context, const Vector2& i_screen_size, Font::Pitch i_pitch, Font::Type i_type, const char* i_font)
	{
		if (!i_context)
			return nullptr;
		Vector2 screen_pixels = Vector2(i_screen_size.x() * i_context->screen_width(), i_screen_size.y() * i_context->screen_height());
		LPD3DXFONT font = nullptr;

		const bool italics = i_type == Font::Type::Italics || i_type == Font::Type::BoldItalics;
		UINT weight;
		if (i_type == Font::Type::Bold || i_type == Font::Type::BoldItalics)
			weight = FW_BOLD;
		else
			weight = FW_NORMAL;
		DWORD pitch = i_pitch == Font::Pitch::Default ? DEFAULT_PITCH : MONO_FONT;
		pitch |= FF_DONTCARE;
		HRESULT result = D3DXCreateFont(
			i_context->get_direct3dDevice(),
			static_cast<UINT>(screen_pixels.y()),
			static_cast<UINT>(screen_pixels.x()),
			weight,
			1,
			italics,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			pitch,
			i_font,
			&font );
		if (FAILED(result))
		{
			return nullptr;
		}

		FontRenderer* fr = new FontRenderer();
		if (fr)
		{
			fr->font = font;
			fr->context_ = i_context;
		}
		else
		{
			font->Release();
		}
		return fr;
	}

	FontRenderer::~FontRenderer()
	{
		LPD3DXFONT f = reinterpret_cast<LPD3DXFONT>(font);
		if (f)
		{
			f->Release();
			f = nullptr;
		}
	}

	bool FontRenderer::Render(const char* i_str, const Rectangle2D& i_screen_rect, Font::HorizontalAlignment i_align, bool i_word_wrap, const Color32& i_color) const
	{
		RECT screen_rect;
		{
			const uint32_t height = context()->screen_height();
			Rectangle2D pixel = context()->GetPixelCoord(i_screen_rect);
			screen_rect.left = static_cast<LONG>(pixel.left());
			screen_rect.right = static_cast<LONG>(pixel.right());
			screen_rect.top = static_cast<LONG>(height - pixel.top());
			screen_rect.bottom = static_cast<LONG>(height - pixel.bottom());
			if (screen_rect.left >= screen_rect.right || screen_rect.top >= screen_rect.bottom)
				return false;
		}
		DWORD format = DT_NOCLIP;
		{
			switch (i_align)
			{
			case Font::HorizontalAlignment::Left:
				format |= DT_LEFT;
				break;
			case Font::HorizontalAlignment::Right:
				format |= DT_RIGHT;
				break;
			case Font::HorizontalAlignment::Center:
				format |= DT_CENTER;
				break;
			default:
				break;
			}
			if (!i_word_wrap)
				format |= DT_WORDBREAK;
		}
		LPD3DXFONT f = reinterpret_cast<LPD3DXFONT>(font);
		INT height = f->DrawTextA(nullptr, i_str, -1, &screen_rect, format, *reinterpret_cast<const D3DCOLOR*>(&i_color));

		DEBUG_PRINT("r(%d, %d, %d, %d) fmt(%d)", screen_rect.left, screen_rect.right, screen_rect.top, screen_rect.bottom, format);
		return height != 0;
	}
}
