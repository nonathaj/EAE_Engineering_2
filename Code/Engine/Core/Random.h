#ifndef _ENGINE_RANDOM_H
#define _ENGINE_RANDOM_H

namespace Engine
{
	namespace Random
	{
		//returns a random number between 0.0f and 1.0f inclusive
		float Value();

		float Range(const float& i_min, const float& i_max);
	}
}

#endif //_ENGINE_RANDOM_H
