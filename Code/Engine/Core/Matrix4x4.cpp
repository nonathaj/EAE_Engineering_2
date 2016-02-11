
#include "Matrix4x4.h"

#include <sstream>

#include "FloatMath.h"
#include "Vector3.h"
#include "Quaternion.h"

namespace
{
	float determinant3x3(const float (&data)[3][3])
	{
		return data[0][0] * data[1][1] * data[2][2] +
			data[0][1] * data[1][2] * data[2][0] +
			data[0][2] * data[1][0] * data[2][1] -
			data[0][2] * data[1][1] * data[2][0] -
			data[0][1] * data[1][0] * data[2][2] -
			data[0][0] * data[1][2] * data[2][1];
	}
}

const Lame::Matrix4x4 Lame::Matrix4x4::identity = Lame::Matrix4x4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1 );

const Lame::Matrix4x4 Lame::Matrix4x4::zero = Lame::Matrix4x4(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0 );

namespace Lame
{
	Matrix4x4::Matrix4x4(const float r1c1, const float r1c2, const float r1c3, const float r1c4,
		const float r2c1, const float r2c2, const float r2c3, const float r2c4,
		const float r3c1, const float r3c2, const float r3c3, const float r3c4,
		const float r4c1, const float r4c2, const float r4c3, const float r4c4)
	{
		Set(0, 0, r1c1);
		Set(0, 1, r1c2);
		Set(0, 2, r1c3);
		Set(0, 3, r1c4);
		Set(1, 0, r2c1);
		Set(1, 1, r2c2);
		Set(1, 2, r2c3);
		Set(1, 3, r2c4);
		Set(2, 0, r3c1);
		Set(2, 1, r3c2);
		Set(2, 2, r3c3);
		Set(2, 3, r3c4);
		Set(3, 0, r4c1);
		Set(3, 1, r4c2);
		Set(3, 2, r4c3);
		Set(3, 3, r4c4);
	}

	Matrix4x4 Matrix4x4::CreateTransformation(const Vector3& i_translation, const Quaternion& i_rotation)
	{
		return CreateTranslation(i_translation) * CreateRotation(i_rotation);
	}

	Matrix4x4 Matrix4x4::CreateTransformation(const Vector3& i_translation, const Quaternion& i_rotation, const Vector3& i_scale)
	{
		return CreateTranslation(i_translation) * CreateRotation(i_rotation) * CreateScale(i_scale);
	}

	Matrix4x4 Matrix4x4::CreateTranslation(float i_x, float i_y, float i_z)
	{
		return Matrix4x4(
			1, 0, 0, i_x,
			0, 1, 0, i_y,
			0, 0, 1, -i_z,
			0, 0, 0, 1);
	}

	Matrix4x4 Matrix4x4::CreateTranslation(const Vector3& i_translation)
	{
		return CreateTranslation(i_translation.x(), i_translation.y(), -i_translation.z());
	}

	Matrix4x4 Matrix4x4::CreateRotationX(const float i_x_degrees)
	{
		float radians = static_cast<float>(Math::ToRadians(i_x_degrees)), v_sin = sin(radians), v_cos = cos(radians);
		return Matrix4x4(
			1, 0, 0, 0,
			0, v_cos, -v_sin, 0,
			0, v_sin, v_cos, 0,
			0, 0, 0, 1 );
	}

	Matrix4x4 Matrix4x4::CreateRotationY(const float i_y_degrees)
	{
		float radians = static_cast<float>(Math::ToRadians(i_y_degrees)), v_sin = sin(radians), v_cos = cos(radians);
		return Matrix4x4(
			v_cos, 0, v_sin, 0,
			0, 1, 0, 0,
			-v_sin, 0, v_cos, 0,
			0, 0, 0, 1 );
	}

	Matrix4x4 Matrix4x4::CreateRotationZ(const float i_z_degrees)
	{
		float radians = static_cast<float>(Math::ToRadians(i_z_degrees)), v_sin = sin(radians), v_cos = cos(radians);
		return Matrix4x4(
			v_cos, -v_sin, 0, 0,
			v_sin, v_cos, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}

	Matrix4x4 Matrix4x4::CreateRotation(float i_x_deg, float i_y_deg, float i_z_deg)
	{
		Matrix4x4 matrix = Matrix4x4::identity;
		if (i_x_deg != 0)
			matrix *= CreateRotationX(i_x_deg);
		if (i_y_deg != 0)
			matrix *= CreateRotationY(i_y_deg);
		if (i_z_deg != 0)
			matrix *= CreateRotationZ(i_z_deg);
		return matrix;
	}

	Matrix4x4 Matrix4x4::CreateRotation(const Vector3& i_euler_angles_degrees)
	{
		return CreateRotation(i_euler_angles_degrees.x(), i_euler_angles_degrees.y(), i_euler_angles_degrees.z());
	}

	Matrix4x4 Matrix4x4::CreateRotation(const Quaternion& i_rotation)
	{
		//http://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix
		Quaternion n = i_rotation.normalized();
		return Matrix4x4(
			1.0f - 2.0f * n.y() * n.y() - 2.0f * n.z() * n.z(), 2.0f * n.x() * n.y() - 2.0f * n.z() * n.w(), 2.0f * n.x() * n.z() + 2.0f * n.y() * n.w(), 0.0f,
			2.0f * n.x() * n.y() + 2.0f * n.z() * n.w(), 1.0f - 2.0f * n.x() * n.x() - 2.0f * n.z() * n.z(), 2.0f * n.y() * n.z() - 2.0f * n.x() * n.w(), 0.0f,
			2.0f * n.x() * n.z() - 2.0f * n.y() * n.w(), 2.0f * n.y() * n.z() + 2.0f * n.x() * n.w(), 1.0f - 2.0f * n.x() * n.x() - 2.0f * n.y() * n.y(), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	Matrix4x4 Matrix4x4::CreateScale(const Vector3& i_scale)
	{
		return CreateScale(i_scale.x(), i_scale.y(), i_scale.z());
	}

	Matrix4x4 Matrix4x4::CreateScale(float i_x, float i_y, float i_z)
	{
		return Matrix4x4(
			i_x, 0.0f, 0.0f, 0.0f,
			0.0f, i_y, 0.0f, 0.0f,
			0.0f, 0.0f, i_z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f );
	}

	Matrix4x4 Matrix4x4::CreateWorldToView(const Vector3& i_cameraPosition, const Quaternion& i_cameraOrientation)
	{
		Matrix4x4 viewToWorld = CreateTransformation(i_cameraPosition, i_cameraOrientation);

		// A camera can only ever have rotation and translation
		// and so a lot of simplifying assumptions can be made in order to create the inverse
		return Matrix4x4(
			viewToWorld.Get(0, 0), viewToWorld.Get(1, 0), viewToWorld.Get(2, 0),
			-(viewToWorld.Get(0, 3) * viewToWorld.Get(0, 0)) - (viewToWorld.Get(1, 3) * viewToWorld.Get(1, 0)) - (viewToWorld.Get(2, 3) * viewToWorld.Get(2, 0)),
			viewToWorld.Get(0, 1), viewToWorld.Get(1, 1), viewToWorld.Get(2, 1),
			-(viewToWorld.Get(0, 3) * viewToWorld.Get(0, 1)) - (viewToWorld.Get(1, 3) * viewToWorld.Get(1, 1)) - (viewToWorld.Get(2, 3) * viewToWorld.Get(2, 1)),
			viewToWorld.Get(0, 2), viewToWorld.Get(1, 2), viewToWorld.Get(2, 2),
			-(viewToWorld.Get(0, 3) * viewToWorld.Get(0, 2)) - (viewToWorld.Get(1, 3) * viewToWorld.Get(1, 2)) - (viewToWorld.Get(2, 3) * viewToWorld.Get(2, 2)),
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	Matrix4x4 Matrix4x4::CreatePerspectiveViewToScreen(const float i_fieldOfView_y, const float i_aspectRatio, const float i_z_nearPlane, const float i_z_farPlane)
	{
		const float yScale = 1.0f / std::tan(i_fieldOfView_y * 0.5f);
		const float xScale = yScale / i_aspectRatio;
#if defined( EAE6320_PLATFORM_D3D )
		const float zDistanceScale = i_z_farPlane / (i_z_nearPlane - i_z_farPlane);
		return Matrix4x4(
			xScale, 0.0f, 0.0f, 0.0f,
			0.0f, yScale, 0.0f, 0.0f,
			0.0f, 0.0f, zDistanceScale, i_z_nearPlane * zDistanceScale,
			0.0f, 0.0f, -1.0f, 0.0f);
#elif defined( EAE6320_PLATFORM_GL )
		const float zDistanceScale = 1.0f / (i_z_nearPlane - i_z_farPlane);
		return Matrix4x4(
			xScale, 0.0f, 0.0f, 0.0f,
			0.0f, yScale, 0.0f, 0.0f,
			0.0f, 0.0f, (i_z_nearPlane + i_z_farPlane) * zDistanceScale, (2.0f * i_z_nearPlane * i_z_farPlane) * zDistanceScale,
			0.0f, 0.0f, -1.0f, 0.0f);
#endif
	}

	Matrix4x4 Matrix4x4::CreateOrthographicViewToScreen(const float i_width, const float i_height, const float i_z_nearPlane, const float i_z_farPlane)
	{
		const float z_dist = i_z_farPlane - i_z_nearPlane;
		return Matrix4x4(
			2.0f / i_width, 0, 0, 0,
			0, 2.0f / i_height, 0, 0,
			0, 0, -1.0f / z_dist, i_z_nearPlane / z_dist,
			0, 0, 0, 1 );
	}

	float Matrix4x4::Determinant() const
	{
		return Get(0, 3) * Get(1, 2) * Get(2, 1) * Get(3, 0) - Get(0, 2) * Get(1, 3) * Get(2, 1) * Get(3, 0) -
			Get(0, 3) * Get(1, 1) * Get(2, 2) * Get(3, 0) + Get(0, 1) * Get(1, 3) * Get(2, 2) * Get(3, 0) +
			Get(0, 2) * Get(1, 1) * Get(2, 3) * Get(3, 0) - Get(0, 1) * Get(1, 2) * Get(2, 3) * Get(3, 0) -
			Get(0, 3) * Get(1, 2) * Get(2, 0) * Get(3, 1) + Get(0, 2) * Get(1, 3) * Get(2, 0) * Get(3, 1) +
			Get(0, 3) * Get(1, 0) * Get(2, 2) * Get(3, 1) - Get(0, 0) * Get(1, 3) * Get(2, 2) * Get(3, 1) -
			Get(0, 2) * Get(1, 0) * Get(2, 3) * Get(3, 1) + Get(0, 0) * Get(1, 2) * Get(2, 3) * Get(3, 1) +
			Get(0, 3) * Get(1, 1) * Get(2, 0) * Get(3, 2) - Get(0, 1) * Get(1, 3) * Get(2, 0) * Get(3, 2) -
			Get(0, 3) * Get(1, 0) * Get(2, 1) * Get(3, 2) + Get(0, 0) * Get(1, 3) * Get(2, 1) * Get(3, 2) +
			Get(0, 1) * Get(1, 0) * Get(2, 3) * Get(3, 2) - Get(0, 0) * Get(1, 1) * Get(2, 3) * Get(3, 2) -
			Get(0, 2) * Get(1, 1) * Get(2, 0) * Get(3, 3) + Get(0, 1) * Get(1, 2) * Get(2, 0) * Get(3, 3) +
			Get(0, 2) * Get(1, 0) * Get(2, 1) * Get(3, 3) - Get(0, 0) * Get(1, 2) * Get(2, 1) * Get(3, 3) -
			Get(0, 1) * Get(1, 0) * Get(2, 2) * Get(3, 3) + Get(0, 0) * Get(1, 1) * Get(2, 2) * Get(3, 3);
	}

	void Matrix4x4::Transpose()
	{
		if (!IsValid())
			return;

		std::swap(data[0][1], data[1][0]);
		std::swap(data[0][2], data[2][0]);
		std::swap(data[0][3], data[3][0]);
		std::swap(data[1][2], data[2][1]);
		std::swap(data[1][3], data[3][1]);
		std::swap(data[2][3], data[3][2]);
	}

	float Matrix4x4::Minor(const size_t i_row, const size_t i_column) const
	{
		if (!IsValid())
			return Math::Float::NaN();

		float minor[3][3];
		size_t row = 0, col;
		for (size_t x = 0; x < 3; x++)
		{
			if (x == i_row)
				row++;
			col = 0;
			for (size_t y = 0; y < 3; y++)
			{
				if (y == i_column)
					col++;

				minor[x][y] = Get(row, col);
				col++;
			}
			row++;
		}
		return determinant3x3(minor);
	}

	Matrix4x4 Matrix4x4::Transposed() const
	{
		if (!IsValid())
			return Matrix4x4();

		Matrix4x4 trans;
		for (size_t x = 0; x < 4; x++)
		{
			for (size_t y = 0; y < 4; y++)
			{
				trans.Set(x, y, Get(y, x));
			}
		}
		return trans;
	}

	Matrix4x4 Matrix4x4::Cofactor() const
	{
		if (!IsValid())
			return Matrix4x4();

		Matrix4x4 matrix;
		for (size_t x = 0; x < 4; x++)
		{
			for (size_t y = 0; y < 4; y++)
			{
				matrix.Set(x, y, Minor(x, y));
				if ((x + y) % 2 == 1)
					matrix.Set(x, y, matrix.Get(x, y) * -1);
			}
		}
		return matrix;
	}

	Matrix4x4 Matrix4x4::Adjugate() const
	{
		if (!IsValid())
			return Matrix4x4();

		Matrix4x4 matrix;
		for (size_t x = 0; x < 4; x++)
		{
			for (size_t y = 0; y < 4; y++)
			{
				matrix.Set(x, y, Minor(y, x));
				if ((x + y) % 2 == 1)
					matrix.Set(x, y, matrix.Get(x, y) * -1);
			}
		}
		return matrix;

		//Simplified version is this (but is slightly slower)
		//return Cofactor().Transposed();
	}

	Matrix4x4 Matrix4x4::Inverse() const
	{
		if (!IsValid())
			return Matrix4x4();

		float determinant = Determinant();
		if (Math::Float::AlmostEqual(determinant, 0.0f))
			return Matrix4x4();
		else
			return Adjugate() * (1.0f / determinant);
	}

	Vector3 Matrix4x4::Multiply(const Vector3& i_rhs, const float i_w) const
	{
		assert(IsValid());
		return Vector3(
			i_rhs.x() * Get(0, 0) + i_rhs.y() * Get(0, 1) + i_rhs.z() * Get(0, 2) + i_w * Get(0, 3),
			i_rhs.x() * Get(1, 0) + i_rhs.y() * Get(1, 1) + i_rhs.z() * Get(1, 2) + i_w * Get(1, 3),
			i_rhs.x() * Get(2, 0) + i_rhs.y() * Get(2, 1) + i_rhs.z() * Get(2, 2) + i_w * Get(2, 3) );
	}

	std::string Matrix4x4::to_string(bool i_format_on_single_line) const
	{
		char break_char = i_format_on_single_line ? ' ' : '\n';
		char open = '{';
		char close = '}';
		std::string separator = ", ";
		std::stringstream mstr;
		mstr << open << break_char <<
			open << ' ' << Get(0, 0) << separator << Get(0, 1) << separator << Get(0, 2) << separator << Get(0, 3) << ' ' << close << break_char <<
			open << ' ' << Get(1, 0) << separator << Get(1, 1) << separator << Get(1, 2) << separator << Get(1, 3) << ' ' << close << break_char <<
			open << ' ' << Get(2, 0) << separator << Get(2, 1) << separator << Get(2, 2) << separator << Get(2, 3) << ' ' << close << break_char <<
			open << ' ' << Get(3, 0) << separator << Get(3, 1) << separator << Get(3, 2) << separator << Get(3, 3) << ' ' << close << break_char <<
			break_char << close;
		return mstr.str();
	}

	Matrix4x4& Matrix4x4::operator*=(const float i_rhs)
	{
		if (IsValid())
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 4; y++)
				{
					Set(x, y, Get(x, y) * i_rhs);
				}
			}
		}
		return *this;
	}

	Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& i_other)
	{
		return *this = *this * i_other;
	}

	Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& i_other)
	{
		if (!IsValid() || !i_other.IsValid())
		{
			Invalidate();
		}
		else
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 4; y++)
				{
					Set(x, y, Get(x, y) + i_other.Get(x, y));
				}
			}
		}
		return *this;
	}

	Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& i_other)
	{
		if (!IsValid() || !i_other.IsValid())
		{
			Invalidate();
		}			
		else
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 4; y++)
				{
					Set(x, y, Get(x, y) - i_other.Get(x, y));
				}
			}
		}
		return *this;
	}

	bool operator==(const Matrix4x4& i_left, const Matrix4x4& i_right)
	{
		if (!i_left.IsValid() || !i_right.IsValid())
		{
			return false;
		}
		else
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 4; y++)
				{
					if (!Math::Float::AlmostEqual(i_left.Get(x, y), i_right.Get(x, y)))
						return false;
				}
			}
			return true;
		}
	}

	bool operator!=(const Matrix4x4& i_left, const Matrix4x4& i_right)
	{
		return !(i_right == i_left);
	}

	Matrix4x4 operator*(const Matrix4x4& i_left, const Matrix4x4& i_right)
	{
		Matrix4x4 result;
		if (i_left.IsValid() && i_right.IsValid())
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 4; y++)
				{
					float val = 0;
					for (size_t z = 0; z < 4; z++)
						val += (i_left.Get(x, z) * i_right.Get(z, y));
					result.Set(x, y, val);
				}
			}
		}
		return result;
	}

	Matrix4x4 operator+(const Matrix4x4& i_left, const Matrix4x4& i_right)
	{
		Matrix4x4 result;
		if (i_left.IsValid() && i_right.IsValid())
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 4; y++)
				{
					result.Set(x, y, i_left.Get(x, y) + i_right.Get(x, y));
				}
			}
		}
		return result;
	}

	Matrix4x4 operator-(const Matrix4x4& i_left, const Matrix4x4& i_right)
	{
		Matrix4x4 result;
		if (i_left.IsValid() && i_right.IsValid())
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 4; y++)
				{
					result.Set(x, y, i_left.Get(x, y) - i_right.Get(x, y));
				}
			}
		}
		return result;
	}

	Matrix4x4 operator*(const Matrix4x4& i_left, const float i_right)
	{
		Matrix4x4 result;
		if (i_left.IsValid())
		{
			for (size_t x = 0; x < 4; x++)
			{
				for (size_t y = 0; y < 4; y++)
				{
					result.Set(x, y, i_left.Get(x, y) * i_right);
				}
			}
		}
		return result;
	}

	Matrix4x4 operator*(const float i_left, const Matrix4x4& i_right)
	{
		return i_right * i_left;
	}
}
