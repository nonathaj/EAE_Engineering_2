#ifndef _ENGINE_SYSTEM_USEROUTPUT_H
#define _ENGINE_SYSTEM_USEROUTPUT_H

#include <string>

namespace Lame
{
	namespace UserOutput
	{
		//Displays a string message to the user as a popup
		void Display(std::string i_messageToUser, std::string i_popupHeader = "Message To User");
	}
}

#endif _ENGINE_SYSTEM_USEROUTPUT_H