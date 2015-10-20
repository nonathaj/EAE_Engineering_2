#ifndef _ENGINE_SYSTEM_USERINPUT_H
#define _ENGINE_SYSTEM_USERINPUT_H

#include <vector>

namespace System
{
	namespace UserInput
	{
		bool KeyPressed(const int i_keyCode);

		bool MouseButtonPressed(const int i_mouseButtonCode);

		//TODO use these instead of int codes
		enum Mouse { Left, Right, Middle };
		enum Key
		{
			A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
			F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
			Space, Esc,
		};
	}
}

#endif _ENGINE_SYSTEM_USERINPUT_H
