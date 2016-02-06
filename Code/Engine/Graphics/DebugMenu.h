#ifndef _LAME_DEBUGMENU_H
#define _LAME_DEBUGMENU_H

#ifdef _DEBUG
#define DEBUG_MENU
#endif

namespace Lame
{
	class DebugMenu
	{
	public:
		void CreateSlider(const char* name, float* value, float min, float max);
		void CreateCheckBox(const char* name, bool* value);
		void CreateText(const char* name, char* value);
		void CreateButton(const char* name, void(*callback)(void*), void *param);
	private:
		DebugMenu();
	};
}

#endif //_LAME_DEBUGMENU_H
