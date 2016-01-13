#ifndef _ENGINE_CORE_MATRIX4X4
#define _ENGINE_CORE_MATRIX4X4

#include "FloatMath.h"

//Default is Column-Major matrices, to switch to row-major, use the #define ENGINE_CORE_MATRIX4X4_USE_ROW_MAJOR

namespace Engine
{
	class Vector3;
	class Quaternion;

	class Matrix4x4
	{
	public:
		static const Matrix4x4 identity;

		inline Matrix4x4() { Invalidate(); }
		Matrix4x4(const float r1c1, const float r1c2, const float r1c3, const float r1c4,
			const float r2c1, const float r2c2, const float r2c3, const float r2c4,
			const float r3c1, const float r3c2, const float r3c3, const float r3c4,
			const float r4c1, const float r4c2, const float r4c3, const float r4c4);

		float Get(const size_t i_row, const size_t i_column);
		void Set(const size_t i_row, const size_t i_column, const float i_value);

		inline void Invalidate() { Set(0, 0, Math::Float::NaN()); }
		inline bool IsValid() { return !Math::Float::IsNaN(Get(0, 0)); }

	private:
		float data[4][4];
	};
}

#endif //_ENGINE_CORE_MATRIX4X4
