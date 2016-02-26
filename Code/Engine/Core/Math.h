#ifndef _ENGINE_MATH_MATH_H
#define _ENGINE_MATH_MATH_H

#include <vector>

namespace Lame
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

		template<typename T>
		inline T Clamp(const T& i_val, const T& i_min, const T& i_max);

		template<typename T>
		inline T Clamp01(const T i_val);

		template<typename T>
		inline T Lerp(const T& i_from, const T& i_to, const float i_t);

		template<typename T>
		inline T LerpUnclamped(const T& i_from, const T& i_to, const float i_t);

		namespace Int
		{
			template<typename T>
			inline bool IsPowerOf2(const T i_int);
		}
	}
}
#include "Math.inl"

#endif //_ENGINE_MATH_MATH_H
