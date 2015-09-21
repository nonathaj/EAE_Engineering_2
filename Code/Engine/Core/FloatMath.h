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
			static bool IsNaN(T i_val);

			//Check if a floating point number is either positive or negative infinity
			template<typename T>
			static bool IsInfinity(T i_val);

			//Check if a floating point number is a NaN or (positive or negative) infinity
			template<typename T>
			static bool IsNaNOrInfinity(T i_val);

			template<typename T>
			static bool AlmostEqual(T i_lhs, T i_rhs);

			template<typename T>
			static bool AlmostEqual(T i_lhs, T i_rhs, T i_epsilon);

			template<typename T>
			static bool AlmostEqualRelative(T i_lhs, T i_rhs);

			template<typename T>
			static bool AlmostEqualRelative(T i_lhs, T i_rhs, T i_maxRelDiff);

			static bool AlmostEqualULP(float i_lhs, float i_rhs, size_t maxULPdiff);
			static bool AlmostEqualULP(double i_lhs, double i_rhs, size_t maxULPdiff);
		}
	}
}
#include "FloatMath.inl"

#endif //_ENGINE_MATH_FLOATMATH_H
