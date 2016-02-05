
#include <vector>
#include <algorithm>
#include <limits>
#include <stdint.h>
#define _USE_MATH_DEFINES
#include <math.h>

namespace Lame
{
	namespace Math
	{
		template<typename T>
		inline double ToDegrees(const T i_radians)
		{
			return i_radians * 180.0 / M_PI;
		}

		template<typename T>
		inline double ToRadians(const T i_degrees)
		{
			return i_degrees * M_PI / 180.0;
		}

		template<typename T>
		inline bool IsNegative(const T i_val)
		{
			return std::numeric_limits<T>::is_signed && (i_val >> (sizeof(T) * CHAR_BIT - 1)) != 0;
		}

		template<typename T>
		inline T& Min(const T i_left, const T i_right)
		{
			return i_left < i_right ? i_left : i_right;
		}

		template<typename T>
		inline T Min(const std::vector<T>& i_list)
		{
			MessagedAssert(i_list.size() > 0, "Cannot find min value from an empty list");
			T min = i_list[0];
			for (size_t x = 1; x < i_list.size(); x++)
				min = Min(min, i_list[x]);
			return min;
		}

		template<typename T>
		inline T& Max(const T i_left, const T i_right)
		{
			return i_left > i_right ? i_left : i_right;
		}

		template<typename T>
		inline T Max(const std::vector<T>& i_list)
		{
			MessagedAssert(i_list.size() > 0, "Cannot find max value from an empty list");
			T max = i_list[0];
			for (size_t x = 1; x < i_list.size(); x++)
				max = Max(max, i_list[x]);
			return max;
		}

		template<typename T>
		inline T Repeat(const T i_num, const T i_lim)
		{
			T result = i_num - ((int)(i_num / i_lim)) * i_lim;
			if (result < 0)
				result += i_lim;
			return result;
		}
		
		namespace Int
		{
			template<typename T>
			inline bool IsPowerOf2(const T i_val)
			{
				volatile T v = i_val;
				return v && !(v & (v - 1));
			}
		}
	}
}
