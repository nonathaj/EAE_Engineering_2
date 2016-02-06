
#include <unordered_map>

#include "UserInput.h"
#include "../Windows/Includes.h"

namespace
{
	bool IsVirtualKeyDown(const int i_virtualKeyCode);

	std::unordered_map<Lame::Input::Keyboard::Key, const int> keyCodes =
	{
		{ Lame::Input::Keyboard::Count, 0 },
		{ Lame::Input::Keyboard::A, 'A' },
		{ Lame::Input::Keyboard::B, 'B' },
		{ Lame::Input::Keyboard::C, 'C' },
		{ Lame::Input::Keyboard::D, 'D' },
		{ Lame::Input::Keyboard::E, 'E' },
		{ Lame::Input::Keyboard::F, 'F' },
		{ Lame::Input::Keyboard::G, 'G' },
		{ Lame::Input::Keyboard::H, 'H' },
		{ Lame::Input::Keyboard::I, 'I' },
		{ Lame::Input::Keyboard::J, 'J' },
		{ Lame::Input::Keyboard::K, 'K' },
		{ Lame::Input::Keyboard::L, 'L' },
		{ Lame::Input::Keyboard::M, 'M' },
		{ Lame::Input::Keyboard::N, 'N' },
		{ Lame::Input::Keyboard::O, 'O' },
		{ Lame::Input::Keyboard::P, 'P' },
		{ Lame::Input::Keyboard::Q, 'Q' },
		{ Lame::Input::Keyboard::R, 'R' },
		{ Lame::Input::Keyboard::S, 'S' },
		{ Lame::Input::Keyboard::T, 'T' },
		{ Lame::Input::Keyboard::U, 'U' },
		{ Lame::Input::Keyboard::V, 'V' },
		{ Lame::Input::Keyboard::W, 'W' },
		{ Lame::Input::Keyboard::X, 'X' },
		{ Lame::Input::Keyboard::Y, 'Y' },
		{ Lame::Input::Keyboard::Z, 'Z' },
		{ Lame::Input::Keyboard::F1, VK_F1 },
		{ Lame::Input::Keyboard::F2, VK_F2 },
		{ Lame::Input::Keyboard::F3, VK_F3 },
		{ Lame::Input::Keyboard::F4, VK_F4 },
		{ Lame::Input::Keyboard::F5, VK_F5 },
		{ Lame::Input::Keyboard::F6, VK_F6 },
		{ Lame::Input::Keyboard::F7, VK_F7 },
		{ Lame::Input::Keyboard::F8, VK_F8 },
		{ Lame::Input::Keyboard::F9, VK_F9 },
		{ Lame::Input::Keyboard::F10, VK_F10 },
		{ Lame::Input::Keyboard::F11, VK_F11 },
		{ Lame::Input::Keyboard::F12, VK_F12 },
		{ Lame::Input::Keyboard::Space, VK_SPACE },
		{ Lame::Input::Keyboard::Esc, VK_ESCAPE },
		{ Lame::Input::Keyboard::Tab, VK_TAB },
		{ Lame::Input::Keyboard::Enter, VK_RETURN },
		{ Lame::Input::Keyboard::Shift, VK_SHIFT },
		{ Lame::Input::Keyboard::Control, VK_CONTROL },
		{ Lame::Input::Keyboard::Up, VK_UP },
		{ Lame::Input::Keyboard::Down, VK_DOWN },
		{ Lame::Input::Keyboard::Left, VK_LEFT },
		{ Lame::Input::Keyboard::Right, VK_RIGHT },
		{ Lame::Input::Keyboard::Alpha0, 0x30 },
		{ Lame::Input::Keyboard::Alpha1, 0x31 },
		{ Lame::Input::Keyboard::Alpha2, 0x32 },
		{ Lame::Input::Keyboard::Alpha3, 0x33 },
		{ Lame::Input::Keyboard::Alpha4, 0x34 },
		{ Lame::Input::Keyboard::Alpha5, 0x35 },
		{ Lame::Input::Keyboard::Alpha6, 0x36 },
		{ Lame::Input::Keyboard::Alpha7, 0x37 },
		{ Lame::Input::Keyboard::Alpha8, 0x38 },
		{ Lame::Input::Keyboard::Alpha9, 0x39 },
		{ Lame::Input::Keyboard::Tilde, VK_OEM_3 },
	};

	std::unordered_map<Lame::Input::Mouse::Button, const int> mouseButtonCodes =
	{
		{ Lame::Input::Mouse::Left, VK_LBUTTON },
		{ Lame::Input::Mouse::Right, VK_RBUTTON },
		{ Lame::Input::Mouse::Middle, VK_MBUTTON },
	};
}

namespace Lame
{
	namespace Input
	{
		void Module::Tick(const float deltaTime)
		{
			for (size_t t = 0; t < Keyboard::Key::Count; t++)
			{
				Keyboard::Key k = static_cast<Keyboard::Key>(t);
				KeyState last = kb_state[k];
				kb_state[k].last_frame = last.this_frame;
				kb_state[k].this_frame = IsVirtualKeyDown(keyCodes[k]);
			}

			for (size_t t = 0; t < Mouse::Button::Count; t++)
			{
				Mouse::Button b = static_cast<Mouse::Button>(t);
				KeyState last = mouse_state[b];
				mouse_state[b].last_frame = last.this_frame;
				mouse_state[b].this_frame = IsVirtualKeyDown(mouseButtonCodes[b]);
			}
		}

		void Module::CursorVisible(bool visible)
		{
			ShowCursor(visible ? TRUE : FALSE);
		}

		bool Module::CursorVisible()
		{
			CURSORINFO cursorInfo;
			if (GetCursorInfo(&cursorInfo))
				return cursorInfo.flags == CURSOR_SHOWING;
			else
				return true;
		}

		bool Module::Up(const Keyboard::Key i_key) const
		{
			if (!enabled())
				return false;
			return UpRaw(i_key);
		}

		bool Module::Up(const Mouse::Button i_button) const
		{
			if (!enabled())
				return false;
			return UpRaw(i_button);
		}

		bool Module::Down(const Keyboard::Key i_key) const
		{
			if (!enabled())
				return false;
			return DownRaw(i_key);
		}

		bool Module::Down(const Mouse::Button i_button) const
		{
			if (!enabled())
				return false;
			return DownRaw(i_button);
		}

		bool Module::Held(const Keyboard::Key i_key) const
		{
			if (!enabled())
				return false;
			return HeldRaw(i_key);
		}

		bool Module::Held(const Mouse::Button i_button) const
		{
			if (!enabled())
				return false;
			return HeldRaw(i_button);
		}

		bool Module::UpRaw(const Keyboard::Key i_key) const
		{
			return !kb_state.at(i_key).this_frame && kb_state.at(i_key).last_frame;
		}

		bool Module::UpRaw(const Mouse::Button i_button) const
		{
			return !mouse_state.at(i_button).this_frame && mouse_state.at(i_button).last_frame;
		}

		bool Module::DownRaw(const Keyboard::Key i_key) const
		{
			return kb_state.at(i_key).this_frame && !kb_state.at(i_key).last_frame;
		}

		bool Module::DownRaw(const Mouse::Button i_button) const
		{
			return mouse_state.at(i_button).this_frame && !mouse_state.at(i_button).last_frame;
		}

		bool Module::HeldRaw(const Keyboard::Key i_key) const
		{
			return kb_state.at(i_key).this_frame;
		}

		bool Module::HeldRaw(const Mouse::Button i_button) const
		{
			return mouse_state.at(i_button).this_frame;
		}
	}
}

namespace
{
	bool IsVirtualKeyDown(const int i_virtualKeyCode)
	{
		short keyState = GetAsyncKeyState(i_virtualKeyCode);
		const short isKeyDownMask = ~1;
		return (keyState & isKeyDownMask) != 0;
	}
}
