#ifndef _ENGINE_MATH_FLOATMATH_H
#define _ENGINE_MATH_FLOATMATH_H

namespace Engine
{
	namespace Math
	{
		namespace Float
		{
			//returns a nan float value
			static float NaN();

			//Check if a floating point number is a NaN value
			template<typename T>
			static bool IsNaN(const T i_val);

			//Check if a floating point number is either positive or negative infinity
			template<typename T>
			static bool IsInfinity(const T i_val);

			//Check if a floating point number is a NaN or (positive or negative) infinity
			template<typename T>
			static bool IsNaNOrInfinity(const T i_val);

			template<typename T>
			static bool AlmostEqual(const T i_lhs, const T i_rhs);

			template<typename T>
			static bool AlmostEqual(const T i_lhs, const T i_rhs, const T i_epsilon);

			template<typename T>
			static bool AlmostEqualRelative(const T i_lhs, const T i_rhs);

			template<typename T>
			static bool AlmostEqualRelative(const T i_lhs, const T i_rhs, const T i_maxRelDiff);

			static bool AlmostEqualULP(const float i_lhs, const float i_rhs, const size_t maxULPdiff);
			static bool AlmostEqualULP(const double i_lhs, const double i_rhs, const size_t maxULPdiff);
		}
	}
}
#include "FloatMath.inl"

#endif //_ENGINE_MATH_FLOATMATH_H
