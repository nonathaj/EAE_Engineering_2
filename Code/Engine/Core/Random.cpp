
#include "Random.h"

#include <cstdlib>
#include <cstdint>

namespace Engine
{
	namespace Random
	{
		float Value()
		{
			//TODO this does not use all of the available mantissa of the floats
			return static_cast<float>(rand()) / static_cast<float>((RAND_MAX + 1));
		}

		float Range(const float& i_min, const float& i_max)
		{
			return i_min + Value() * (i_max - i_min);
		}
	}
}
