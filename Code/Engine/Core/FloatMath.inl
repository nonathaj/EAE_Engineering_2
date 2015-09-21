
#include <algorithm>
#include <limits>
#include <stdint.h>
#include "Math.h"

namespace Engine
{
	namespace Math
	{
		namespace Float
		{
			float NaN()
			{
				return static_cast<float>(sqrt(-1));
			}

			template<typename T>
			bool IsNaN(T i_val)
			{
				volatile T v = i_val;
				return v != v;
			}

			template<typename T>
			bool IsInfinity(T i_val)
			{
				volatile T v = i_val;
				return (v > 0 || v < 0) && (v / v != v / v);
			}

			template<typename T>
			bool IsNaNOrInfinity(T i_val)
			{
				volatile T v = i_val;
				return (v * 0) != 0;
			}

			template<typename T>
			bool AlmostEqual(T i_lhs, T i_rhs)
			{
				return AlmostEqual(i_lhs, i_rhs, std::numeric_limits<T>::epsilon());
			}

			template<typename T>
			bool AlmostEqual(T i_lhs, T i_rhs, T i_epsilon)
			{
				return fabs(i_lhs - i_rhs) <= i_epsilon;
			}

			template<typename T>
			bool AlmostEqualRelative(T i_lhs, T i_rhs)
			{
				return AlmostEqualRelative(i_lhs, i_rhs, std::numeric_limits<T>::epsilon());
			}

			template<typename T>
			bool AlmostEqualRelative(T i_lhs, T i_rhs, T i_maxRelDiff)
			{
				T diff = fabs(i_lhs - i_rhs);
				i_lhs = fabs(i_lhs);
				i_rhs = fabs(i_rhs);
				return diff <= std::max(i_lhs, i_rhs) * i_maxRelDiff;
			}

			bool AlmostEqualULP(float i_lhs, float i_rhs, size_t maxULPdiff)
			{
				//treat each float as an integer
				int32_t left = *((int32_t*)&i_lhs);
				int32_t right = *((int32_t*)&i_rhs);

				//if they have different signs, they are not equal (unless both are zero)
				if (Engine::Math::IsNegative(left) != Engine::Math::IsNegative(right))
					return i_lhs == i_rhs;

				//Compare the ULP difference to our maxULP difference allowed
				return ((uint32_t)abs(left - right)) <= maxULPdiff;
			}

			bool AlmostEqualULP(double i_lhs, double i_rhs, size_t maxULPdiff)
			{
				//treat each double as an integer
				int64_t left = *((int64_t*)&i_lhs);
				int64_t right = *((int64_t*)&i_rhs);

				//if they have different signs, they are not equal (unless both are zero)
				if (IsNegative(left) != IsNegative(right))
					return i_lhs == i_rhs;

				//Compare the ULP difference to our maxULP difference allowed
				return abs(left - right) <= maxULPdiff;
			}
		}
	}
}
