#include "Matrix4x4.h"

const Engine::Matrix4x4 Engine::Matrix4x4::identity = Engine::Matrix4x4(
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1 );

namespace Engine
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

	float Matrix4x4::Get(const size_t i_row, const size_t i_column)
	{
#if defined(ENGINE_CORE_MATRIX4X4_USE_ROW_MAJOR)
		return data[i_row][i_column];
#else
		return data[i_column][i_row];
#endif
	}

	void Matrix4x4::Set(const size_t i_row, const size_t i_column, const float i_value)
	{
#if defined(ENGINE_CORE_MATRIX4X4_USE_ROW_MAJOR)
		data[i_row][i_column] = i_value;
#else
		data[i_column][i_row] = i_value;
#endif
	}
}
