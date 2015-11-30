
#include <map>

#include "UserInput.h"
#include "../Windows/Includes.h"

namespace
{
	bool IsVirtualKeyPressed(const int i_virtualKeyCode);

	std::map<System::UserInput::Keyboard::Key, const int> keyCodes =
	{
		{ System::UserInput::Keyboard::A, 'A' },
		{ System::UserInput::Keyboard::B, 'B' },
		{ System::UserInput::Keyboard::C, 'C' },
		{ System::UserInput::Keyboard::D, 'D' },
		{ System::UserInput::Keyboard::E, 'E' },
		{ System::UserInput::Keyboard::F, 'F' },
		{ System::UserInput::Keyboard::G, 'G' },
		{ System::UserInput::Keyboard::H, 'H' },
		{ System::UserInput::Keyboard::I, 'I' },
		{ System::UserInput::Keyboard::J, 'J' },
		{ System::UserInput::Keyboard::K, 'K' },
		{ System::UserInput::Keyboard::L, 'L' },
		{ System::UserInput::Keyboard::M, 'M' },
		{ System::UserInput::Keyboard::N, 'N' },
		{ System::UserInput::Keyboard::O, 'O' },
		{ System::UserInput::Keyboard::P, 'P' },
		{ System::UserInput::Keyboard::Q, 'Q' },
		{ System::UserInput::Keyboard::R, 'R' },
		{ System::UserInput::Keyboard::S, 'S' },
		{ System::UserInput::Keyboard::T, 'T' },
		{ System::UserInput::Keyboard::U, 'U' },
		{ System::UserInput::Keyboard::V, 'V' },
		{ System::UserInput::Keyboard::W, 'W' },
		{ System::UserInput::Keyboard::X, 'X' },
		{ System::UserInput::Keyboard::Y, 'Y' },
		{ System::UserInput::Keyboard::Z, 'Z' },
		{ System::UserInput::Keyboard::F1, VK_F1 },
		{ System::UserInput::Keyboard::F2, VK_F2 },
		{ System::UserInput::Keyboard::F3, VK_F3 },
		{ System::UserInput::Keyboard::F4, VK_F4 },
		{ System::UserInput::Keyboard::F5, VK_F5 },
		{ System::UserInput::Keyboard::F6, VK_F6 },
		{ System::UserInput::Keyboard::F7, VK_F7 },
		{ System::UserInput::Keyboard::F8, VK_F8 },
		{ System::UserInput::Keyboard::F9, VK_F9 },
		{ System::UserInput::Keyboard::F10, VK_F10 },
		{ System::UserInput::Keyboard::F11, VK_F11 },
		{ System::UserInput::Keyboard::F12, VK_F12 },
		{ System::UserInput::Keyboard::Space, VK_SPACE },
		{ System::UserInput::Keyboard::Esc, VK_ESCAPE },
		{ System::UserInput::Keyboard::Tab, VK_TAB },
		{ System::UserInput::Keyboard::Enter, VK_RETURN },
		{ System::UserInput::Keyboard::Shift, VK_SHIFT },
		{ System::UserInput::Keyboard::Control, VK_CONTROL },
		{ System::UserInput::Keyboard::Up, VK_UP },
		{ System::UserInput::Keyboard::Down, VK_DOWN },
		{ System::UserInput::Keyboard::Left, VK_LEFT },
		{ System::UserInput::Keyboard::Right, VK_RIGHT },
	};

	std::map<System::UserInput::Mouse::Button, const int> mouseButtonCodes =
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
		namespace Keyboard
		{
			bool Pressed(const Key i_key)
			{
				return IsVirtualKeyPressed(keyCodes[i_key]);
			}
		}

		namespace Mouse
		{
			bool Pressed(const Button i_mouse_button)
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
