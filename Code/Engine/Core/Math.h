#ifndef _ENGINE_MATH_MATH_H
#define _ENGINE_MATH_MATH_H

#include <vector>

namespace Engine
{
	namespace Math
	{
		template<typename T>
		inline double ToDegrees(const T i_radians);

		template<typename T>
		inline double ToRadians(const T i_degrees);

		//Check if a signed (or floating point) number is negative
		template<typename T>
		inline bool IsNegative(const T i_val);

		template<typename T>
		inline T& Min(const T i_left, const T i_right);

		template<typename T>
		inline T Min(const std::vector<T>& i_list);

		template<typename T>
		inline T& Max(const T i_left, const T i_right);

		template<typename T>
		inline T Max(const std::vector<T>& i_list);

		template<typename T>
		inline T Repeat(const T i_num, const T i_lim);

		namespace Int
		{
			template<typename T>
			inline bool IsPowerOf2(const T i_int);
		}
	}
}
#include "Math.inl"

#endif //_ENGINE_MATH_MATH_H
