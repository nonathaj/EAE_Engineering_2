
#include "../FontRenderer.h"

#include "../Context.h"
#include "../../Core/Rectangle2D.h"
#include "../../Core/Vector2.h"

namespace Lame
{
	FontRenderer* FontRenderer::Create(std::shared_ptr<Lame::Context> i_context, const Vector2& i_screen_size, Font::Pitch i_pitch, Font::Type i_type)
	{
		if (!i_context)
			return nullptr;
		Vector2 screen_pixels = Vector2(i_screen_size.x() / i_context->screen_width(), i_screen_size.y() / i_context->screen_height());
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
			screen_pixels.y(),
			screen_pixels.x(),
			weight,
			1,
			italics,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			pitch,
			"",
			&font );
		if (FAILED(result))
		{
			return nullptr;
		}

		FontRenderer* fr = new FontRenderer();
		if (fr)
		{
			fr->font = font;
		}
		else
		{
			font->Release();
		}
		return fr;
	}

	FontRenderer::~FontRenderer()
	{
		if (font)
		{
			font->Release();
			font = nullptr;
		}
	}
}
