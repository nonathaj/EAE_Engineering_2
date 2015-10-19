
#include "Effect.h"

namespace Lame
{
	char const * const Effect::PositionUniformName = "position_offset";
	const Engine::HashedString Effect::PositionUniformId(Effect::PositionUniformName);

	bool Effect::SetPosition(Engine::Vector2 i_position)
	{
		return SetConstant(PositionUniformId, i_position);
	}
}