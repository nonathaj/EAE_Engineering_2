
#include <stdarg.h>		// for va_<xxx>
#include <stdio.h>		// for vsprintf_s()

#include "..\Windows\Includes.h"
#include "Console.h"

namespace Core
{
	void ConsolePrint(bool i_displayFileAndLine, const char * i_file, const unsigned int i_line, const char * i_fmt, ...)
	{
		const size_t lenTemp = 1024;
		char strTemp[lenTemp];

		if (i_displayFileAndLine)
			sprintf_s(strTemp, "DEBUG (%s Line # %d): %s\n", i_file, i_line, i_fmt);	//add the file, line number, and format string
		else
			sprintf_s(strTemp, "DEBUG: %s\n", i_fmt);									//only add the format string

		const size_t lenOutput = lenTemp + 32;

		char strOutput[lenOutput];

		// define a variable argument list variable 
		va_list args;
	
		// initialize it. second parameter is variable immediately
		// preceeding variable arguments
		va_start(args, i_fmt);

		// (safely) print our formatted string to a char buffer
		vsprintf_s(strOutput, lenOutput, strTemp, args);

		va_end(args);

		OutputDebugStringA(strOutput);
	}

} // namespace Engine