
namespace Lame
{
	namespace Time
	{
		namespace //Private members for Time
		{
			Tick previousFrameTick_;
			Tick currentFrameTick_;
			Tick startTick_;
		}

		inline double SecsToMS(double i_seconds)
		{
			return i_seconds * 1000.0;
		}

		inline double MSToSecs(double i_milliseconds)
		{
			return i_milliseconds / 1000.0;
		}

		inline double DifferenceSecs(Tick i_start, Tick i_end)
		{
			return TickToSecs(i_end - i_start);
		}

		inline double DifferenceMS(Tick i_start, Tick i_end)
		{
			return TickToMS(i_end - i_start);
		}

		//Returns the elapsed time between the current frame and the previous frame
		inline double DeltaTime()
		{
			return DifferenceSecs(previousFrameTick_, CurrentFrameTick());
		}

		//Returns the elapsed time since the game started
		inline double ElapsedTime() 
		{
			return DifferenceSecs(StartFrameTick(), CurrentFrameTick());
		}

		//Returns the current frame system time
		inline Tick CurrentFrameTick() 
		{
			return currentFrameTick_;
		}

		//Returns when the game was marked to begin
		inline Tick StartFrameTick() 
		{
			return startTick_;
		}

		//Marks the beginning of the programs run time
		inline void Setup() 
		{ 
			previousFrameTick_ = currentFrameTick_ = startTick_ = GetCurrentSystemTick();
		}

		//Updates the current and previous frame times
		inline void Service()
		{
			previousFrameTick_ = currentFrameTick_;
			currentFrameTick_ = GetCurrentSystemTick();
		}
	}
}