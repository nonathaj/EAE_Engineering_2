#ifndef _TOOLS_MATERIALBUILDER_H
#define _TOOLS_MATERIALBUILDER_H

#include "../BuilderHelper/cbBuilder.h"

class MaterialBuilder : public eae6320::cbBuilder
{
public:
	virtual bool Build(const std::vector<std::string>& i_arguments);
};


#endif //_TOOLS_MATERIALBUILDER_H
