
#include "Effect.h"

namespace Lame
{
	char const * const Effect::PositionUniformName = "position_offset";
	const Engine::HashedString Effect::PositionUniformId(Effect::PositionUniformName);

	bool Effect::SetPosition(eae6320::Math::cVector i_position)
	{
		return SetConstant(PositionUniformId, i_position);
	}
}
