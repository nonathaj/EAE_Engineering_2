
#include <map>

#include "UserInput.h"
#include "../Windows/Includes.h"

namespace
{
	bool IsVirtualKeyPressed(const int i_virtualKeyCode);

	std::map<System::UserInput::Key::Key, const int> keyCodes =
	{
		{ System::UserInput::Key::A, 'A' },
		{ System::UserInput::Key::B, 'B' },
		{ System::UserInput::Key::C, 'C' },
		{ System::UserInput::Key::D, 'D' },
		{ System::UserInput::Key::E, 'E' },
		{ System::UserInput::Key::F, 'F' },
		{ System::UserInput::Key::G, 'G' },
		{ System::UserInput::Key::H, 'H' },
		{ System::UserInput::Key::I, 'I' },
		{ System::UserInput::Key::J, 'J' },
		{ System::UserInput::Key::K, 'K' },
		{ System::UserInput::Key::L, 'L' },
		{ System::UserInput::Key::M, 'M' },
		{ System::UserInput::Key::N, 'N' },
		{ System::UserInput::Key::O, 'O' },
		{ System::UserInput::Key::P, 'P' },
		{ System::UserInput::Key::Q, 'Q' },
		{ System::UserInput::Key::R, 'R' },
		{ System::UserInput::Key::S, 'S' },
		{ System::UserInput::Key::T, 'T' },
		{ System::UserInput::Key::U, 'U' },
		{ System::UserInput::Key::V, 'V' },
		{ System::UserInput::Key::W, 'W' },
		{ System::UserInput::Key::X, 'X' },
		{ System::UserInput::Key::Y, 'Y' },
		{ System::UserInput::Key::Z, 'Z' },
		{ System::UserInput::Key::F1, VK_F1 },
		{ System::UserInput::Key::F2, VK_F2 },
		{ System::UserInput::Key::F3, VK_F3 },
		{ System::UserInput::Key::F4, VK_F4 },
		{ System::UserInput::Key::F5, VK_F5 },
		{ System::UserInput::Key::F6, VK_F6 },
		{ System::UserInput::Key::F7, VK_F7 },
		{ System::UserInput::Key::F8, VK_F8 },
		{ System::UserInput::Key::F9, VK_F9 },
		{ System::UserInput::Key::F10, VK_F10 },
		{ System::UserInput::Key::F11, VK_F11 },
		{ System::UserInput::Key::F12, VK_F12 },
		{ System::UserInput::Key::Space, VK_SPACE },
		{ System::UserInput::Key::Esc, VK_ESCAPE },
		{ System::UserInput::Key::Tab, VK_TAB },
		{ System::UserInput::Key::Enter, VK_RETURN },
		{ System::UserInput::Key::Shift, VK_SHIFT },
		{ System::UserInput::Key::Control, VK_CONTROL },
		{ System::UserInput::Key::Up, VK_UP },
		{ System::UserInput::Key::Down, VK_DOWN },
		{ System::UserInput::Key::Left, VK_LEFT },
		{ System::UserInput::Key::Right, VK_RIGHT },
	};

	std::map<System::UserInput::Mouse::Mouse, const int> mouseButtonCodes =
	{
		{ System::UserInput::Mouse::Left, VK_LBUTTON },
		{ System::UserInput::Mouse::Right, VK_RBUTTON },
		{ System::UserInput::Mouse::Middle, VK_MBUTTON },
	};
}

namespace System
{
	namespace UserInput
	{
		bool KeyPressed(const Key::Key i_key)
		{
			return IsVirtualKeyPressed(keyCodes[i_key]);
		}

		bool MouseButtonPressed(const Mouse::Mouse i_mouse_button)
		{
			return IsVirtualKeyPressed(mouseButtonCodes[i_mouse_button]);
		}

		void SetCursorVisible(bool visible)
		{
			ShowCursor(visible ? TRUE : FALSE);
		}

		bool IsCursorVisible()
		{
			CURSORINFO cursorInfo;
			if (GetCursorInfo(&cursorInfo))
				return cursorInfo.flags == CURSOR_SHOWING;
			else
				return true;
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
