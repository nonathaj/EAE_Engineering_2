
#include "Context.h"

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
		uint32_t width = screen_width();
		uint32_t height = screen_height();
		if(width >= height)
			return static_cast<float>(width) / static_cast<float>(height);
		else
			return static_cast<float>(height) / static_cast<float>(width);
	}
}
