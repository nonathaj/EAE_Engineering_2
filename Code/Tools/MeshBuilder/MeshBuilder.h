/*
	This is an example builder program that just copies the source file to the target path
	(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef TOOLS_MESHBUILDER_MESHBUILDER_H
#define TOOLS_MESHBUILDER_MESHBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"

// Class Declaration
//==================

namespace eae6320
{
	class MeshBuilder : public cbBuilder
	{
		// Interface
		//==========

	public:

		// Build
		//------

		virtual bool Build( const std::vector<std::string>& i_arguments );
	};
}

#endif	// TOOLS_MESHBUILDER_MESHBUILDER_H
