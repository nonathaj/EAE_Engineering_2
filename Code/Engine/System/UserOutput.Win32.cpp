
#include "UserOutput.h"
#include "../Windows/Includes.h"

namespace Lame
{
	namespace UserOutput
	{
		void Display(std::string i_messageToUser, std::string i_popupHeader)
		{
			MessageBox(nullptr, i_messageToUser.c_str(), i_popupHeader.c_str(), 0);
		}
	}
}