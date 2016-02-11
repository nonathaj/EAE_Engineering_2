#ifndef _ENGINE_CORE_MATRIX4X4
#define _ENGINE_CORE_MATRIX4X4

#include <cassert>
#include <string>

#include "FloatMath.h"

namespace Lame
{
	class Vector3;
	class Quaternion;

	class Matrix4x4
	{
	public:
		static const Matrix4x4 identity;
		static const Matrix4x4 zero;

		inline Matrix4x4() { Invalidate(); }
 		Matrix4x4(const float r1c1, const float r2c1, const float r3c1, const float r4c1,
			const float r1c2, const float r2c2, const float r3c2, const float r4c2,
			const float r1c3, const float r2c3, const float r3c3, const float r4c3,
			const float r1c4, const float r2c4, const float r3c4, const float r4c4);

		static Matrix4x4 CreateTransformation(const Vector3& i_translation, const Quaternion& i_rotation);
		static Matrix4x4 CreateTransformation(const Vector3& i_translation, const Quaternion& i_rotation, const Vector3& i_scale);
		static Matrix4x4 CreateTranslation(float i_x, float i_y, float i_z);
		static Matrix4x4 CreateTranslation(const Vector3& i_translation);
		static Matrix4x4 CreateRotationX(const float i_x_degrees);
		static Matrix4x4 CreateRotationY(const float i_y_degrees);
		static Matrix4x4 CreateRotationZ(const float i_z_degrees);
		static Matrix4x4 CreateRotation(float i_x_deg, float i_y_deg, float i_z_deg);
		static Matrix4x4 CreateRotation(const Vector3& i_euler_angles_degrees);
		static Matrix4x4 CreateRotation(const Quaternion& i_rotation);
		static Matrix4x4 CreateScale(const Vector3& i_scale);
		static Matrix4x4 CreateScale(float i_x, float i_y, float i_z);
		static Matrix4x4 CreateWorldToView(const Vector3& i_cameraPosition, const Quaternion& i_cameraOrientation);
		static Matrix4x4 CreatePerspectiveViewToScreen(const float i_fieldOfView_y, const float i_aspectRatio, const float i_z_nearPlane, const float i_z_farPlane);
		static Matrix4x4 CreateOrthographicViewToScreen(const float i_width, const float i_height, const float i_z_nearPlane, const float i_z_farPlane);

		inline float Get(const size_t i_row, const size_t i_column) const;
		inline void Set(const size_t i_row, const size_t i_column, const float i_value);

		inline void Invalidate() { Set(0, 0, Math::Float::NaN()); }
		inline bool IsValid() const { return !Math::Float::IsNaN(Get(0, 0)); }

		void Transpose();

		float Determinant() const;
		inline float Trace() const;
		Matrix4x4 Transposed() const;
		Matrix4x4 Cofactor() const;
		Matrix4x4 Adjugate() const;
		Matrix4x4 Inverse() const;

		float Minor(const size_t i_row, const size_t i_column) const;

		Vector3 Multiply(const Vector3& i_rhs, const float i_w = 1.0f) const;

		std::string to_string(bool i_format_on_single_line = true) const;

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

namespace Lame
{
	inline float Matrix4x4::Get(const size_t i_row, const size_t i_column) const
	{
		assert(i_row >= 0 && i_row < 4);
		assert(i_column >= 0 && i_column < 4);
		return data[i_row][i_column];
	}

	inline void Matrix4x4::Set(const size_t i_row, const size_t i_column, const float i_value)
	{
		assert(i_row >= 0 && i_row < 4);
		assert(i_column >= 0 && i_column < 4);
		data[i_row][i_column] = i_value;
	}

	inline float Matrix4x4::Trace() const
	{
		return Get(0, 0) + Get(1, 1) + Get(2, 2) + Get(3, 3);
	}
}

#endif //_ENGINE_CORE_MATRIX4X4
