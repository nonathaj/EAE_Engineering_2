
#include "Matrix4x4.h"

#include "FloatMath.h"

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

const Engine::Matrix4x4 Engine::Matrix4x4::identity = Engine::Matrix4x4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1 );

const Engine::Matrix4x4 Engine::Matrix4x4::zero = Engine::Matrix4x4(
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0 );

namespace Engine
{
	Matrix4x4::Matrix4x4(const float(&i_data)[16]) :
		Matrix4x4(i_data[0], i_data[1], i_data[2], i_data[3],
		i_data[4], i_data[5], i_data[6], i_data[7],
		i_data[8], i_data[9], i_data[10], i_data[11],
		i_data[12], i_data[13], i_data[14], i_data[15])
	{
	}

	Matrix4x4::Matrix4x4(const float r1c1, const float r2c1, const float r3c1, const float r4c1,
		const float r1c2, const float r2c2, const float r3c2, const float r4c2,
		const float r1c3, const float r2c3, const float r3c3, const float r4c3,
		const float r1c4, const float r2c4, const float r3c4, const float r4c4)
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
		//Matrix4x4 adj = Cofactor().Transposed();
		//return adj;
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
		bool leftValid = i_left.IsValid();
		bool rightValid = i_right.IsValid();
		if (!leftValid && !rightValid)
		{
			return true;
		}
		else if (!leftValid || !rightValid)
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
