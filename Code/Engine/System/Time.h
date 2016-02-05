#ifndef _ENGINE_SYSTEM_TIME_H
#define _ENGINE_SYSTEM_TIME_H

#include <stdint.h>

/*
	Time namespace

	Used for tracking the time in high precision during gameplay.

	Tracks the deltaTime since the last frame, and the elapsed time since the game began.
*/
namespace Lame
{
	namespace Time
	{
		//arbitrary counter for time
		typedef uint64_t Tick;

		//PLATFORM INDEPENDENT - returns the length of a given tick in MS/Seconds
		double TickToSecs(Tick i_ticks);
		double TickToMS(Tick i_ticks);

		//MS/Seconds conversion functions
		double SecsToMS(double i_seconds);
		double MSToSecs(double i_milliseconds);

		//Tick to MS/Seconds comparison functions
		inline double DifferenceSecs(Tick i_start, Tick i_end);
		inline double DifferenceMS(Tick i_start, Tick i_end);

		//PLATFORM INDEPENDENT - Returns the exact current system time in ticks
		Tick GetCurrentSystemTick();

		//Returns the elapsed time between the current frame and the previous frame
		inline double DeltaTime();

		//Returns the elapsed time since the game started
		inline double ElapsedTime();

		//Returns the current frame system time
		inline Tick CurrentFrameTick();

		//Returns when the game was marked to begin
		inline Tick StartFrameTick();

		//Marks the beginning of the programs run time
		inline void Setup();

		//Updates internal time values for this frame
		inline void Service();
	}
}
#include "Time.inl"

#endif //_ENGINE_SYSTEM_TIME_H