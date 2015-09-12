/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#endif //EAE6320_PLATFORM_D3D

// Header Files
//=============

#include "../../Engine/Windows/Includes.h"

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		bool Initialize( const HWND i_renderingWindow );
		void Render();
		bool ShutDown();

#if EAE6320_PLATFORM_D3D
		IDirect3DDevice9* get_direct3dDevice();
#endif //EAE6320_PLATFORM_D3D
	}
}

#endif	// EAE6320_GRAPHICS_H
