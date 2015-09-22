#ifndef _ENGINE_MATH_MATH_H
#define _ENGINE_MATH_MATH_H

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
		inline T ToDegrees(T i_radians);

		template<typename T>
		inline T ToRadians(T i_degrees);

		//Check if a signed (or floating point) number is negative
		template<typename T>
		inline bool IsNegative(T i_val);

		template<typename T>
		inline T& Min(T &i_left, T &i_right);

		template<typename T>
		inline T Min(std::vector<T> i_list);

		template<typename T>
		inline T& Max(T &i_left, T &i_right);

		template<typename T>
		inline T Max(std::vector<T> i_list);

		template<typename T>
		inline T Repeat(T i_num, T i_lim);

		template<typename T>
		inline T Clamp(T i_num, T i_min, T i_max);

		template<typename T>
		inline T Lerp(T i_num, T i_target, float i_percent);

		template<typename T>
		inline bool IsWithinRange(T i_num, T i_min, T i_max);

		namespace Int
		{
			template<typename T>
			inline bool IsPowerOf2(T i_int);
		}
	}
}

////////////////////////////////////////////////////////////
// Implementation
////////////////////////////////////////////////////////////

namespace Engine
{
	namespace Math
	{
		template<typename T>
		inline T ToDegrees(T i_radians)
		{
			return static_cast<T>(i_radians * (180.0 / M_PI));
		}

		template<typename T>
		inline T ToRadians(T i_degrees)
		{
			return static_cast<T>(i_degrees * (M_PI / 180.0));
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
			T result = i_num - floor(i_num / i_lim) * i_lim;
			if (result < 0)
				result += i_lim;
			return result;
		}


		template<typename T>
		inline T Clamp(T i_num, T i_min, T i_max)
		{
			if (i_num <= i_min)
				return i_min;
			if (i_num >= i_max)
				return i_max;
			return i_num;
		}

		template<typename T>
		inline T Lerp(T i_num, T i_target, float i_percent)
		{
			i_percent = Clamp(i_percent, 0.0f, 1.0f);
			return i_num + (i_target - i_num) * i_percent;
		}

		template<typename T>
		inline bool IsWithinRange(T i_num, T i_min, T i_max)
		{
			return i_num >= i_min && i_num <= i_max;
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

#endif //_ENGINE_MATH_MATH_H
