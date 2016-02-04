#ifndef _ENGINE_SYSTEM_USERINPUT_H
#define _ENGINE_SYSTEM_USERINPUT_H

#include <vector>

namespace System
{
	namespace UserInput
	{
		namespace Mouse 
		{ 
			enum Button { Left, Right, Middle };
			bool Pressed(const Button i_mouse_button);

			void SetCursorVisible(bool visible);
			bool IsCursorVisible();
		}

		namespace Keyboard
		{
			enum Key
			{
				A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
				F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
				Space, Esc, Tab, Enter, Shift, Control,
				Up, Down, Left, Right,
				Alpha0, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,
			};
			bool Pressed(const Key i_key);
		}
	}
}

#endif _ENGINE_SYSTEM_USERINPUT_H
