#ifndef _ENGINE_SYSTEM_USERINPUT_H
#define _ENGINE_SYSTEM_USERINPUT_H

#include <vector>
#include <unordered_map>

#include "../Core/Singleton.h"

namespace Lame
{
	namespace Input
	{
		namespace Mouse { enum Button { Left, Right, Middle, Count }; }

		namespace Keyboard
		{
			enum Key
			{
				A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
				F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
				Space, Esc, Tab, Enter, Shift, Control, Tilde,
				Up, Down, Left, Right,
				Alpha0, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9, 
				Count
			};
		}

		class Module
		{
		public:
			bool Up(const Keyboard::Key i_key) const;
			bool Up(const Mouse::Button i_button) const;
			bool Down(const Keyboard::Key i_key) const;
			bool Down(const Mouse::Button i_button) const;
			bool Held(const Keyboard::Key i_key) const;
			bool Held(const Mouse::Button i_button) const;

			//ignores disabled state
			bool UpRaw(const Keyboard::Key i_key) const;
			bool UpRaw(const Mouse::Button i_button) const;
			bool DownRaw(const Keyboard::Key i_key) const;
			bool DownRaw(const Mouse::Button i_button) const;
			bool HeldRaw(const Keyboard::Key i_key) const;
			bool HeldRaw(const Mouse::Button i_button) const;

			bool enabled() const { return enabled_; }
			void enabled(bool i_enabled) { enabled_ = i_enabled; }

			void CursorVisible(bool visible);
			bool CursorVisible();

			void Tick(const float deltaTime);
		private:
			Module() : enabled_(true) {}

			struct KeyState
			{
				bool last_frame;
				bool this_frame;
				KeyState() : last_frame(false), this_frame(false) {}
				KeyState(const bool last, const bool thisf) : last_frame(last), this_frame(thisf) {}
			};

			bool enabled_;
			std::unordered_map<Lame::Input::Keyboard::Key, KeyState> kb_state;
			std::unordered_map<Lame::Input::Mouse::Button, KeyState> mouse_state;

			friend Lame::Singleton<Lame::Input::Module>;
		};
	}
}

typedef Lame::Singleton<Lame::Input::Module> LameInput;

#endif _ENGINE_SYSTEM_USERINPUT_H
