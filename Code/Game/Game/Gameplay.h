#ifndef _GAME_GAMEPLAY_H
#define _GAME_GAMEPLAY_H

#include "../../Engine/Windows/Includes.h"

namespace Gameplay
{
	bool Initialize(HWND i_window);
	bool RunFrame();
	bool Shutdown();
}

#endif //_GAME_GAMEPLAY_H