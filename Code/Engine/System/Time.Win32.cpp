
#include <cassert>

#include "../Windows/Includes.h"
#include "Time.h"

namespace Engine
{
	namespace Time
	{
		namespace //private members for time on Win32
		{
			Tick freq = 0;

			void CheckWin32Setup()
			{
				if(freq == 0)
				{
					LARGE_INTEGER frequency;
					assert(QueryPerformanceFrequency(&frequency));
					freq = frequency.QuadPart;
				}
			}
		}

		double TickToMS(Tick i_ticks)
		{
			CheckWin32Setup();
			double freqInMS = static_cast<double>(freq) * 1000.0;
			return static_cast<double>(i_ticks) / freqInMS;
		}

		double TickToSecs(Tick i_ticks)
		{
			CheckWin32Setup();
			return static_cast<double>(i_ticks) / static_cast<double>(freq);
		}

		Tick GetCurrentSystemTick()
		{
			LARGE_INTEGER now;
			assert(QueryPerformanceCounter(&now));
			return static_cast<Tick>(now.QuadPart);
		}
	}
}