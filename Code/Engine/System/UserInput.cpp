
#include "UserInput.h"
#include "../Windows/Includes.h"

namespace
{
	bool IsVirtualKeyPressed(const int i_virtualKeyCode);
}

namespace Engine
{
	namespace UserInput
	{
		bool KeyPressed(const int i_keyCode)
		{
			return IsVirtualKeyPressed(i_keyCode);
		}

		bool MouseButtonPressed(const int i_mouseButtonCode)
		{
			return IsVirtualKeyPressed(i_mouseButtonCode);
		}
	}
}

namespace
{
	bool IsVirtualKeyPressed(const int i_virtualKeyCode)
	{
		short keyState = GetAsyncKeyState(i_virtualKeyCode);
		const short isKeyDownMask = ~1;
		return (keyState & isKeyDownMask) != 0;
	}
}
