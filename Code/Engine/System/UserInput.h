#ifndef _ENGINE_SYSTEM_USERINPUT_H
#define _ENGINE_SYSTEM_USERINPUT_H

#include <vector>

namespace System
{
	namespace UserInput
	{
		namespace Mouse { enum Mouse { Left, Right, Middle }; }
		namespace Key
		{
			enum Key
			{
				A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
				F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
				Space, Esc, Tab, Enter, Shift, Control,
				Up, Down, Left, Right,
			};
		}
		
		bool KeyPressed(const Key::Key i_key);
		bool MouseButtonPressed(const Mouse::Mouse i_mouse_button);

		void SetCursorVisible(bool visible);
		bool IsCursorVisible();
	}
}

#endif _ENGINE_SYSTEM_USERINPUT_H
