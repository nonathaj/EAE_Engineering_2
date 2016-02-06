
#include "Context.h"

#include "../Core/Rectangle2D.h"

namespace Lame
{

	Context::Context(HWND i_renderingWindow) :
		renderingWindow(i_renderingWindow),
		screen_clear_color(0.0f, 0.0f, 0.0f, 1.0f)
	{ }

	uint32_t Context::screen_width() const
	{
		RECT rect;
		if (GetWindowRect(renderingWindow, &rect))
			return static_cast<uint32_t>(rect.right - rect.left);
		return 0;
	}

	uint32_t Context::screen_height() const
	{
		RECT rect;
		if (GetWindowRect(renderingWindow, &rect))
			return static_cast<uint32_t>(rect.bottom - rect.top);
		return 0;
	}

	float Context::aspect_ratio() const
	{
		RECT rect;
		if (GetWindowRect(renderingWindow, &rect))
			return static_cast<float>(rect.right - rect.left) / static_cast<float>(rect.bottom - rect.top);
		return 0;
	}

	Rectangle2D Context::GetPixelCoord(const Rectangle2D& i_virtual_screen_coord)
	{
		uint32_t width = screen_width();
		uint32_t height = screen_height();
		return Rectangle2D(
			i_virtual_screen_coord.left() * width,
			i_virtual_screen_coord.right() * width,
			i_virtual_screen_coord.top() * height,
			i_virtual_screen_coord.bottom() * height );
	}
}
