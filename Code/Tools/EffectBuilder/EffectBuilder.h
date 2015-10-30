#ifndef _TOOLS_EFFECTBUILDER_EFFECTBUILDER_H
#define _TOOLS_EFFECTBUILDER_EFFECTBUILDER_H

#include "../BuilderHelper/cbBuilder.h"

class EffectBuilder : public eae6320::cbBuilder
{
public:
	virtual bool Build(const std::vector<std::string>& i_arguments);
};

#endif //_TOOLS_EFFECTBUILDER_EFFECTBUILDER_H