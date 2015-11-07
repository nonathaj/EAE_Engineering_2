
#include "Context.h"

namespace Lame
{
	uint32_t Context::screen_width()
	{
		RECT rect;
		if (GetWindowRect(renderingWindow, &rect))
			return static_cast<uint32_t>(rect.right - rect.left);
		return 0;
	}

	uint32_t Context::screen_height()
	{
		RECT rect;
		if (GetWindowRect(renderingWindow, &rect))
			return static_cast<uint32_t>(rect.bottom - rect.top);
		return 0;
	}
}
