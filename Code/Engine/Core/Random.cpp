
#include "Random.h"

#include <cstdlib>
#include <cstdint>
#include <type_traits>
#include <random>
#include <chrono>

namespace
{
	unsigned default_seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
	std::default_random_engine generator(default_seed);
	std::uniform_real_distribution<float> distribution(0.0f, 1.0f + std::numeric_limits<float>::epsilon());
}

namespace Lame
{
	namespace Random
	{
		float Value()
		{
			// construct a trivial random generator engine from a time-based seed:
			static unsigned seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
			static std::default_random_engine generator(seed);
			static std::uniform_real_distribution<float> distribution(0.0f, 1.0f + std::numeric_limits<float>::epsilon());

			return distribution(generator);

			//TODO this might be a more efficient way to calculate random numbers,
			//	BUT does not use all of the available mantissa of the floats
			//return static_cast<float>(rand()) / static_cast<float>((RAND_MAX + 1));
		}

		float Range(const float& i_min, const float& i_max)
		{
			return i_min + Value() * (i_max - i_min);
		}

		size_t Range(const size_t& i_min, const size_t& i_max_exclusive)
		{
			return static_cast<size_t>( static_cast<float>(i_min) + Value() * static_cast<float>(i_max_exclusive - i_min - 1) );
		}
	}
}
