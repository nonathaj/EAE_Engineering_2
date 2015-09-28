// Header Files
//=============

#include "UtilityFunctions.h"
#include "../../Engine/System/Console.h"

#include <iostream>
#include <sstream>

// Interface
//==========

void eae6320::OutputErrorMessage( const char* const i_errorMessage, const char* const i_optionalFileName )
{
	std::stringstream err;
	err << (i_optionalFileName ? i_optionalFileName : "Asset Build") << ": error: " << i_errorMessage << "\n";
	// This formatting causes the errors to show up in Visual Studio's "Error List" tab
	std::cerr << err.str();
}
