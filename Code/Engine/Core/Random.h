#ifndef _ENGINE_RANDOM_H
#define _ENGINE_RANDOM_H

#include <vector>

namespace Engine
{
	namespace Random
	{
		//returns a random number between 0.0f and 1.0f inclusive
		float Value();

		float Range(const float& i_min, const float& i_max);
		size_t Range(const size_t& i_min, const size_t& i_max_exclusive);
	}
}

#endif //_ENGINE_RANDOM_H
