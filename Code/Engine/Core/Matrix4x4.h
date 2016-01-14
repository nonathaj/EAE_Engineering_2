#ifndef _ENGINE_CORE_MATRIX4X4
#define _ENGINE_CORE_MATRIX4X4

#include <cassert>

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
		static const Matrix4x4 zero;

		inline Matrix4x4() { Invalidate(); }
		inline Matrix4x4(const float(&i_data)[4][4]) : Matrix4x4(reinterpret_cast<const float(&)[16]>(i_data)) {}
		Matrix4x4(const float(&i_data)[16]);
		Matrix4x4(const float r1c1, const float r2c1, const float r3c1, const float r4c1,
			const float r1c2, const float r2c2, const float r3c2, const float r4c2,
			const float r1c3, const float r2c3, const float r3c3, const float r4c3,
			const float r1c4, const float r2c4, const float r3c4, const float r4c4);

		inline float Get(const size_t i_row, const size_t i_column) const;
		inline void Set(const size_t i_row, const size_t i_column, const float i_value);

		inline void Invalidate() { Set(0, 0, Math::Float::NaN()); }
		inline bool IsValid() const { return !Math::Float::IsNaN(Get(0, 0)); }

		float Determinant() const;
		inline float Trace() const;
		void Transpose();

		Matrix4x4 Transposed() const;
		Matrix4x4 Cofactor() const;
		Matrix4x4 Adjugate() const;
		Matrix4x4 Inverse() const;

		float Minor(const size_t i_row, const size_t i_column) const;

		Matrix4x4& operator*=(const Matrix4x4& i_other);
		Matrix4x4& operator+=(const Matrix4x4& i_other);
		Matrix4x4& operator-=(const Matrix4x4& i_other);
		Matrix4x4& operator*=(const float i_rhs);

	private:
		float data[4][4];
	};

	bool operator==(const Matrix4x4& i_left, const Matrix4x4& i_right);
	bool operator!=(const Matrix4x4& i_left, const Matrix4x4& i_right);
	
	Matrix4x4 operator*(const Matrix4x4& i_left, const Matrix4x4& i_right);
	Matrix4x4 operator+(const Matrix4x4& i_left, const Matrix4x4& i_right);
	Matrix4x4 operator-(const Matrix4x4& i_left, const Matrix4x4& i_right);

	Matrix4x4 operator*(const Matrix4x4& i_left, const float i_right);
	Matrix4x4 operator*(const float i_left, const Matrix4x4& i_right);
}

namespace Engine
{
	float Matrix4x4::Get(const size_t i_row, const size_t i_column) const
	{
		assert(i_row >= 0 && i_row < 4);
		assert(i_column >= 0 && i_column < 4);

#if defined(ENGINE_CORE_MATRIX4X4_USE_ROW_MAJOR)
		return data[i_row][i_column];
#else
		return data[i_column][i_row];
#endif
	}

	void Matrix4x4::Set(const size_t i_row, const size_t i_column, const float i_value)
	{
		assert(i_row >= 0 && i_row < 4);
		assert(i_column >= 0 && i_column < 4);

#if defined(ENGINE_CORE_MATRIX4X4_USE_ROW_MAJOR)
		data[i_row][i_column] = i_value;
#else
		data[i_column][i_row] = i_value;
#endif
	}

	float Matrix4x4::Trace() const
	{
		return Get(0, 0) + Get(1, 1) + Get(2, 2) + Get(3, 3);
	}
}

#endif //_ENGINE_CORE_MATRIX4X4