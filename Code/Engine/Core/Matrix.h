#ifndef _ENGINE_MATH_MATRIX_H
#define _ENGINE_MATH_MATRIX_H

#include <stdint.h>
#include <vector>
#include "FloatMath.h"

namespace Engine
{
	template<size_t Rows, size_t Columns>
	class Matrix
	{
	public:
		inline Matrix() { Invalidate(); }
		inline Matrix(const Matrix<Rows, Columns> &i_other);
		static void SetIdentity(Matrix<Rows, Columns> &o_matrix);

		inline void Invalidate() { Set(0, 0, Math::Float::NaN()); }
		inline bool IsValid() const { return !Math::Float::IsNaN(Get(0, 0)); }
		inline float Get(size_t i_row, size_t i_column) const { return data[i_row][i_column]; }
		inline void Set(size_t i_row, size_t i_column, float i_value);

		inline void SetRow(size_t i_row, const std::vector<float> &i_vals);

		inline Matrix<Rows - 1, Columns - 1> RemoveRowColumn(size_t row, size_t column) const;

		inline float Determinant() const;
		inline float Trace() const;
		inline Matrix<Columns, Rows> Transposed() const;
		inline Matrix<Rows, Rows> Cofactor() const;
		inline Matrix<Rows, Rows> Adjugate() const;
		inline Matrix<Rows, Rows> Inverse() const;
		inline bool IsSymmetric() const;
		inline bool IsSkewSymmetric() const;
		inline bool IsOrthogonal() const;
		inline bool IsDiagonal() const;
		inline bool IsUpperTriangular() const;
		inline bool IsLowerTriangular() const;
		inline bool IsInvertible() const;
		inline bool IsSquare() const { return Rows == Columns; }

		inline void RowAdd(size_t from_row, size_t to_row);
		inline void RowMultiply(size_t row, float val);
		inline void RowSwap(size_t row1, size_t row2);

		inline Matrix<Rows, Columns>& operator=(const Matrix<Rows, Columns> &rhs);
		inline Matrix<Rows, Columns>& operator+=(const Matrix<Rows, Columns> &rhs);
		inline Matrix<Rows, Columns>& operator*=(float rhs);
		inline Matrix<Rows, Columns>& operator*=(const Matrix<Rows, Columns> &rhs);
	private:
		float data[Rows][Columns];
	};

	template<size_t Rows, size_t Columns> inline bool					operator==(const Matrix<Rows, Columns> &lhs, const Matrix<Rows, Columns> &rhs);
	template<size_t Rows, size_t Columns> inline bool					operator!=(const Matrix<Rows, Columns> &lhs, const Matrix<Rows, Columns> &rhs);
	template<size_t Rows, size_t Columns> inline Matrix<Rows, Columns>	operator+(const Matrix<Rows, Columns> &lhs, const Matrix<Rows, Columns> &rhs);
	template<size_t Rows, size_t Columns> inline Matrix<Rows, Columns>	operator*(const Matrix<Rows, Columns> &lhs, float rhs);
	template<size_t Rows, size_t Columns> inline Matrix<Rows, Columns>	operator*(float lhs, const Matrix<Rows, Columns> &rhs);
	template<size_t Rows, size_t Columns> inline std::ostream&			operator<<(std::ostream &out, const Matrix<Rows, Columns> &i_matrix);

	template<size_t LHS_Rows, size_t LC_RR, size_t RHS_Columns>
	inline Matrix<LHS_Rows, RHS_Columns> operator*(const Matrix<LHS_Rows, LC_RR> &lhs, const Matrix<LC_RR, RHS_Columns> &rhs);


	//////////////////////////////////////////////////////////////////////////////////
	//		Definitions
	//////////////////////////////////////////////////////////////////////////////////

	//define the matrix at 0, 0 so that it stops the recursive definition at compile time
	template<> class Matrix < 0, 0 > 
	{
	public:
		inline bool IsValid() const { return false; }
		inline float Determinant() const { return Engine::Math::Float::NaN(); }
		inline void Set(size_t i_row, size_t i_column, float i_value) {}
	private:
		float data[1][1];
	};

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Columns>::Matrix(const Matrix<Rows, Columns> &i_other)
	{
		if (i_other.IsValid())
		{
			for (size_t x = 0; x < Rows; x++)
				for (size_t y = 0; y < Columns; y++)
					Set(x, y, i_other.Get(x, y));
		}
		else
			Invalidate();
	}

	template<size_t Rows, size_t Columns>
	inline void Matrix<Rows, Columns>::Set(size_t i_row, size_t i_column, float i_value)
	{
		if (i_row < Rows && i_column < Columns)
			data[i_row][i_column] = i_value;
	}

	template<size_t Rows, size_t Columns>
	inline void Matrix<Rows, Columns>::SetRow(size_t i_row, const std::vector<float> &i_vals)
	{
		if (i_row >= Rows)
			return;

		for (size_t x = 0; x < Columns; x++)
		{
			if (x < i_vals.size())
				Set(i_row, x, i_vals[x]);
			else
				Set(i_row, x, 0.0f);
		}
	}

	template<size_t Rows, size_t Columns>
	void Matrix<Rows, Columns>::SetIdentity(Matrix<Rows, Columns> &o_matrix)
	{
		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				o_matrix.Set(x, y, x == y ? 1.0f : 0.0f);
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows-1, Columns-1> Matrix<Rows, Columns>::RemoveRowColumn(size_t i_row, size_t i_column) const
	{
		Matrix<Rows - 1, Columns - 1> matrix;
		if (IsValid())
		{
			size_t row = 0, col = 0;
			for (size_t x = 0; x < Rows - 1; x++)
			{
				if (row == i_row) 
					row++;
				col = 0;
				for (size_t y = 0; y < Columns - 1; y++)
				{
					if (col == i_column) 
						col++;
					matrix.Set(x, y, Get(row, col));
					col++;
				}
				row++;
			}
		}
		return matrix;
	}

	template<size_t Rows, size_t Columns>
	inline float Matrix<Rows, Columns>::Determinant() const
	{
		if (!IsValid() || !IsSquare() || Rows <= 0)
			return Math::Float::NaN();

		if (Rows == 1)
			return Get(0, 0);
		else if (Rows == 2)
			return Get(0, 0) * Get(1, 1) - Get(0, 1) * Get(1, 0);
		else
		{
			float determinant = 0;
			for (size_t y = 0; y < Columns; y++)
			{
				if (y % 2 == 0)
					determinant += Get(0, y) * RemoveRowColumn(0, y).Determinant();
				else
					determinant -= Get(0, y) * RemoveRowColumn(0, y).Determinant();
			}
			return determinant;
		}
	}

	template<size_t Rows, size_t Columns>
	inline float Matrix<Rows, Columns>::Trace() const
	{
		if (!IsValid())
			return Math::Float::NaN();

		float trace = 0;
		size_t min = Rows < Columns ? Rows : Columns;
		for (size_t x = 0; x < min; x++)
			trace += Get(x, x);
		return trace;
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Rows> Matrix<Rows, Columns>::Cofactor() const
	{
		Matrix<Rows, Rows> matrix;
		if (!IsValid() || !IsSquare() || Rows <= 0)
			return matrix;

		for (size_t x = 0; x < Rows; x++)
		{
			for (size_t y = 0; y < Columns; y++)
			{
				matrix.Set(x, y, RemoveRowColumn(x, y).Determinant());
				if ((x + y) % 2 == 1)
					matrix.Set(x, y, matrix.Get(x, y) * -1);
			}
		}
		return matrix;
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Rows> Matrix<Rows, Columns>::Adjugate() const
	{
		Matrix<Rows, Rows> matrix;
		if (!IsValid() || !IsSquare() || Rows <= 0)
			return matrix;

		return Cofactor().Transposed();
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Rows> Matrix<Rows, Columns>::Inverse() const
	{
		Matrix<Rows, Rows> matrix;
		if (!IsValid() || !IsSquare() || Rows <= 0)
			return matrix;
		
		float determinant = Determinant();
		if (determinant == 0 || Math::Float::IsNaN(determinant))
			return matrix;

		return Adjugate() * (1 / determinant);
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Columns, Rows> Matrix<Rows, Columns>::Transposed() const
	{
		Matrix<Columns, Rows> matrix;
		if (IsValid())
		{
			for (size_t x = 0; x < Rows; x++)
				for (size_t y = 0; y < Columns; y++)
					matrix.Set(y, x, Get(x, y));
		}
		return matrix;
	}

	template<size_t Rows, size_t Columns>
	inline bool Matrix<Rows, Columns>::IsSymmetric() const
	{
		return IsValid() && IsSquare() && *this == Transposed();
	}

	template<size_t Rows, size_t Columns>
	inline bool Matrix<Rows, Columns>::IsSkewSymmetric() const
	{
		return IsValid() && IsSquare() && *this == (Transposed() * -1);
	}

	template<size_t Rows, size_t Columns>
	inline bool Matrix<Rows, Columns>::IsOrthogonal() const
	{
		return IsValid() && IsSquare() && Transposed() == Inverse();
	}

	template<size_t Rows, size_t Columns>
	inline bool Matrix<Rows, Columns>::IsDiagonal() const
	{
		if (!IsValid())
			return false;
		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				if (x != y && Get(x, y) != 0)
					return false;
		return true;
	}

	template<size_t Rows, size_t Columns>
	inline bool Matrix<Rows, Columns>::IsUpperTriangular() const
	{
		if (!IsValid())
			return false;
		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				if (x < y && Get(x, y) != 0)
					return false;
		return true;
	}

	template<size_t Rows, size_t Columns>
	inline bool Matrix<Rows, Columns>::IsLowerTriangular() const
	{
		if (!IsValid())
			return false;
		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				if (x > y && Get(x, y) != 0)
					return false;
		return true;
	}

	template<size_t Rows, size_t Columns>
	inline bool Matrix<Rows, Columns>::IsInvertible() const
	{
		if (!IsValid() || !IsSquare() || Rows <= 0)
			return false;
		Matrix<Rows, Rows> identity;
		SetIdentity(identity);
		return *this * Inverse() == identity;
	}

	template<size_t Rows, size_t Columns>
	inline void Matrix<Rows, Columns>::RowAdd(size_t from_row, size_t to_row)
	{
		MessagedAssert(IsValid(), "Attempting operation on invalid matrix.");
		for (size_t y = 0; y < Columns; y++)
			Set(to_row, y, Get(from_row, y) + Get(to_row, y));
	}

	template<size_t Rows, size_t Columns>
	inline void Matrix<Rows, Columns>::RowMultiply(size_t row, float val)
	{
		MessagedAssert(IsValid(), "Attempting operation on invalid matrix.");
		for (size_t y = 0; y < Columns; y++)
			Set(row, y, Get(row, y) * val);
	}

	template<size_t Rows, size_t Columns>
	inline void Matrix<Rows, Columns>::RowSwap(size_t row1, size_t row2)
	{
		MessagedAssert(IsValid(), "Attempting operation on invalid matrix.");
		float temp;
		for (size_t y = 0; y < Columns; y++)
		{
			temp = Get(row1, y);
			Set(row1, y, Get(row2, y));
			Set(row2, y, temp);
		}
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Columns>& Matrix<Rows, Columns>::operator=(const Matrix<Rows, Columns> &rhs)
	{
		if (!rhs.IsValid())
		{
			Invalidate();
			return *this;
		}

		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				Set(x, y, rhs.Get(x, y));
		return *this;
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Columns>& Matrix<Rows, Columns>::operator+=(const Matrix<Rows, Columns> &rhs)
	{
		MessagedAssert(IsValid() && rhs.IsValid(), "Attempting operations on invalid matrices.");
		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				Set(x, y, Get(x, y) + rhs.Get(x, y));
		return *this;
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Columns>& Matrix<Rows, Columns>::operator*=(float rhs)
	{
		MessagedAssert(IsValid() && rhs.IsValid(), "Attempting operations on invalid matrices.");
		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				Set(x, y, Get(x, y) * rhs);
		return *this;
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Columns>& Matrix<Rows, Columns>::operator*=(const Matrix<Rows, Columns> &rhs)
	{
		MessagedAssert(IsValid() && rhs.IsValid(), "Attempting operations on invalid matrices.");
		return *this = *this * rhs;
	}

	template<size_t Rows, size_t Columns>
	inline bool operator==(const Matrix<Rows, Columns> &lhs, const Matrix<Rows, Columns> &rhs)
	{
		if (!lhs.IsValid() && !rhs.IsValid())
			return true;

		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				if (!Math::Float::AlmostEqual(lhs.Get(x, y), rhs.Get(x, y)))
					return false;
		return true;
	}

	template<size_t Rows, size_t Columns>
	inline bool operator!=(const Matrix<Rows, Columns> &lhs, const Matrix<Rows, Columns> &rhs)
	{
		return !(lhs == rhs);
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Columns> operator+(const Matrix<Rows, Columns> &lhs, const Matrix<Rows, Columns> &rhs)
	{
		MessagedAssert(lhs.IsValid() && rhs.IsValid(), "Attempting operations on invalid matrices.");
		Matrix<Rows, Columns> matrix;
		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				matrix.Set(x, y, lhs.Get(x, y) + rhs.Get(x, y));
		return matrix;
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Columns> operator*(const Matrix<Rows, Columns> &lhs, float rhs)
	{
		MessagedAssert(lhs.IsValid(), "Attempting operation on invalid matrix.");
		Matrix<Rows, Columns> matrix;
		for (size_t x = 0; x < Rows; x++)
			for (size_t y = 0; y < Columns; y++)
				matrix.Set(x, y, lhs.Get(x, y) * rhs);
		return matrix;
	}

	template<size_t Rows, size_t Columns>
	inline Matrix<Rows, Columns> operator*(float lhs, const Matrix<Rows, Columns> &rhs)
	{
		MessagedAssert(rhs.IsValid(), "Attempting operation on invalid matrix.");
		return rhs * lhs;
	}

	template<size_t LHS_Rows, size_t LC_RR, size_t RHS_Columns>
	inline Matrix<LHS_Rows, RHS_Columns> operator*(const Matrix<LHS_Rows, LC_RR> &lhs, const Matrix<LC_RR, RHS_Columns> &rhs)
	{
		MessagedAssert(lhs.IsValid() && rhs.IsValid(), "Attempting operations on invalid matrices.");
		Matrix<LHS_Rows, RHS_Columns> result;
		for (size_t x = 0; x < LHS_Rows; x++)
		{
			for (size_t y = 0; y < RHS_Columns; y++)
			{
				float val = 0;
				for (size_t z = 0; z < LC_RR; z++)
					val += (lhs.Get(x, z) * rhs.Get(z, y));
				result.Set(x, y, val);
			}
		}
		return result;
	}

	template<size_t Rows, size_t Columns> 
	inline std::ostream& operator<<(std::ostream &out, const Matrix<Rows, Columns> &i_matrix)
	{
		MessagedAssert(i_matrix.IsValid(), "Attempting operation on invalid matrix.");
		out << "{ ";
		for (size_t x = 0; x < Rows; x++)
		{
		out << "{ ";
		for (size_t y = 0; y < Columns; y++)
		{
			out << i_matrix.Get(x, y);
			if (y < Columns - 1)
				out << ", ";
		}
		out << " }";
		if (x < Rows - 1)
			out << ", ";
		}

		out << " }";
		return out;
	}
}

#endif //_ENGINE_MATH_MATRIX_H