
#include <algorithm>
#include <limits>
#include <stdint.h>
#include "Math.h"

namespace Lame
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
			bool IsNaN(const T i_val)
			{
				volatile T v = i_val;
				return v != v;
			}

			template<typename T>
			bool IsInfinity(const T i_val)
			{
				volatile T v = i_val;
				return (v > 0 || v < 0) && (v / v != v / v);
			}

			template<typename T>
			bool IsNaNOrInfinity(const T i_val)
			{
				volatile T v = i_val;
				return (v * 0) != 0;
			}

			template<typename T>
			bool AlmostEqual(const T i_lhs, const T i_rhs)
			{
				return AlmostEqual(i_lhs, i_rhs, std::numeric_limits<T>::epsilon());
			}

			template<typename T>
			bool AlmostEqual(const T i_lhs, const T i_rhs, const T i_epsilon)
			{
				return fabs(i_lhs - i_rhs) <= i_epsilon;
			}

			template<typename T>
			bool AlmostEqualRelative(const T i_lhs, const T i_rhs)
			{
				return AlmostEqualRelative(i_lhs, i_rhs, std::numeric_limits<T>::epsilon());
			}

			template<typename T>
			bool AlmostEqualRelative(const T i_lhs, const T i_rhs, const T i_maxRelDiff)
			{
				const T diff = fabs(i_lhs - i_rhs);
				i_lhs = fabs(i_lhs);
				i_rhs = fabs(i_rhs);
				return diff <= std::max(i_lhs, i_rhs) * i_maxRelDiff;
			}

			bool AlmostEqualULP(const float i_lhs, const float i_rhs, const size_t maxULPdiff)
			{
				//treat each float as an integer
				const int32_t left = *reinterpret_cast<const int32_t*>(&i_lhs);
				const int32_t right = *reinterpret_cast<const int32_t*>(&i_rhs);

				//if they have different signs, they are not equal (unless both are zero)
				if (Lame::Math::IsNegative(left) != Lame::Math::IsNegative(right))
					return i_lhs == i_rhs;

				//Compare the ULP difference to our maxULP difference allowed
				return static_cast<size_t>(abs(left - right)) <= maxULPdiff;
			}

			bool AlmostEqualULP(const double i_lhs, const double i_rhs, const size_t maxULPdiff)
			{
				//treat each double as an integer
				const int64_t left = *reinterpret_cast<const int64_t*>(&i_lhs);
				const int64_t right = *reinterpret_cast<const int64_t*>(&i_rhs);

				//if they have different signs, they are not equal (unless both are zero)
				if (IsNegative(left) != IsNegative(right))
					return i_lhs == i_rhs;

				//Compare the ULP difference to our maxULP difference allowed
				return static_cast<size_t>(abs(left - right)) <= maxULPdiff;
			}
		}
	}
}
