#ifndef _ENGINE_MATH_MATH_H
#define _ENGINE_MATH_MATH_H

#include <vector>

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
#include "Math.inl"

#endif //_ENGINE_MATH_MATH_H
