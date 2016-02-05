
#include <unordered_map>

#include "UserInput.h"
#include "../Windows/Includes.h"

namespace
{
	bool IsVirtualKeyPressed(const int i_virtualKeyCode);

	std::unordered_map<Lame::UserInput::Keyboard::Key, const int> keyCodes =
	{
		{ Lame::UserInput::Keyboard::A, 'A' },
		{ Lame::UserInput::Keyboard::B, 'B' },
		{ Lame::UserInput::Keyboard::C, 'C' },
		{ Lame::UserInput::Keyboard::D, 'D' },
		{ Lame::UserInput::Keyboard::E, 'E' },
		{ Lame::UserInput::Keyboard::F, 'F' },
		{ Lame::UserInput::Keyboard::G, 'G' },
		{ Lame::UserInput::Keyboard::H, 'H' },
		{ Lame::UserInput::Keyboard::I, 'I' },
		{ Lame::UserInput::Keyboard::J, 'J' },
		{ Lame::UserInput::Keyboard::K, 'K' },
		{ Lame::UserInput::Keyboard::L, 'L' },
		{ Lame::UserInput::Keyboard::M, 'M' },
		{ Lame::UserInput::Keyboard::N, 'N' },
		{ Lame::UserInput::Keyboard::O, 'O' },
		{ Lame::UserInput::Keyboard::P, 'P' },
		{ Lame::UserInput::Keyboard::Q, 'Q' },
		{ Lame::UserInput::Keyboard::R, 'R' },
		{ Lame::UserInput::Keyboard::S, 'S' },
		{ Lame::UserInput::Keyboard::T, 'T' },
		{ Lame::UserInput::Keyboard::U, 'U' },
		{ Lame::UserInput::Keyboard::V, 'V' },
		{ Lame::UserInput::Keyboard::W, 'W' },
		{ Lame::UserInput::Keyboard::X, 'X' },
		{ Lame::UserInput::Keyboard::Y, 'Y' },
		{ Lame::UserInput::Keyboard::Z, 'Z' },
		{ Lame::UserInput::Keyboard::F1, VK_F1 },
		{ Lame::UserInput::Keyboard::F2, VK_F2 },
		{ Lame::UserInput::Keyboard::F3, VK_F3 },
		{ Lame::UserInput::Keyboard::F4, VK_F4 },
		{ Lame::UserInput::Keyboard::F5, VK_F5 },
		{ Lame::UserInput::Keyboard::F6, VK_F6 },
		{ Lame::UserInput::Keyboard::F7, VK_F7 },
		{ Lame::UserInput::Keyboard::F8, VK_F8 },
		{ Lame::UserInput::Keyboard::F9, VK_F9 },
		{ Lame::UserInput::Keyboard::F10, VK_F10 },
		{ Lame::UserInput::Keyboard::F11, VK_F11 },
		{ Lame::UserInput::Keyboard::F12, VK_F12 },
		{ Lame::UserInput::Keyboard::Space, VK_SPACE },
		{ Lame::UserInput::Keyboard::Esc, VK_ESCAPE },
		{ Lame::UserInput::Keyboard::Tab, VK_TAB },
		{ Lame::UserInput::Keyboard::Enter, VK_RETURN },
		{ Lame::UserInput::Keyboard::Shift, VK_SHIFT },
		{ Lame::UserInput::Keyboard::Control, VK_CONTROL },
		{ Lame::UserInput::Keyboard::Up, VK_UP },
		{ Lame::UserInput::Keyboard::Down, VK_DOWN },
		{ Lame::UserInput::Keyboard::Left, VK_LEFT },
		{ Lame::UserInput::Keyboard::Right, VK_RIGHT },
		{ Lame::UserInput::Keyboard::Alpha0, 0x30 },
		{ Lame::UserInput::Keyboard::Alpha1, 0x31 },
		{ Lame::UserInput::Keyboard::Alpha2, 0x32 },
		{ Lame::UserInput::Keyboard::Alpha3, 0x33 },
		{ Lame::UserInput::Keyboard::Alpha4, 0x34 },
		{ Lame::UserInput::Keyboard::Alpha5, 0x35 },
		{ Lame::UserInput::Keyboard::Alpha6, 0x36 },
		{ Lame::UserInput::Keyboard::Alpha7, 0x37 },
		{ Lame::UserInput::Keyboard::Alpha8, 0x38 },
		{ Lame::UserInput::Keyboard::Alpha9, 0x39 },
	};

	std::unordered_map<Lame::UserInput::Mouse::Button, const int> mouseButtonCodes =
	{
		{ Lame::UserInput::Mouse::Left, VK_LBUTTON },
		{ Lame::UserInput::Mouse::Right, VK_RBUTTON },
		{ Lame::UserInput::Mouse::Middle, VK_MBUTTON },
	};
}

namespace Lame
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
