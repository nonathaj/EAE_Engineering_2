
#include <vector>
#include <algorithm>
#include <limits>
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>

namespace Engine
{
	namespace Math
	{
		template<typename T>
		inline double ToDegrees(T i_radians)
		{
			return i_radians * 180.0 / M_PI;
		}

		template<typename T>
		inline double ToRadians(T i_degrees)
		{
			return i_degrees * M_PI / 180.0;
		}

		template<typename T>
		inline bool IsNegative(T i_val)
		{
			return std::numeric_limits<T>::is_signed && (i_val >> (sizeof(T) * CHAR_BIT - 1)) != 0;
		}

		template<typename T>
		inline T& Min(T &i_left, T &i_right)
		{
			return i_left < i_right ? i_left : i_right;
		}

		template<typename T>
		inline T Min(std::vector<T> i_list)
		{
			MessagedAssert(i_list.size() > 0, "Cannot find min value from an empty list");
			T min = i_list[0];
			for (size_t x = 1; x < i_list.size(); x++)
				min = Min(min, i_list[x]);
			return min;
		}

		template<typename T>
		inline T& Max(T &i_left, T &i_right)
		{
			return i_left > i_right ? i_left : i_right;
		}

		template<typename T>
		inline T Max(std::vector<T> i_list)
		{
			MessagedAssert(i_list.size() > 0, "Cannot find max value from an empty list");
			T max = i_list[0];
			for (size_t x = 1; x < i_list.size(); x++)
				max = Max(max, i_list[x]);
			return max;
		}

		template<typename T>
		inline T Repeat(T i_num, T i_lim)
		{
			T result = i_num - ((int)(i_num / i_lim)) * i_lim;
			if (result < 0)
				result += i_lim;
			return result;
		}
		
		namespace Int
		{
			template<typename T>
			inline bool IsPowerOf2(T i_val)
			{
				volatile T v = i_val;
				return v && !(v & (v - 1));
			}
		}
	}
}
