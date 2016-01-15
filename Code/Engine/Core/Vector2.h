#ifndef _ENGINE_MATH_VECTOR2_H
#define _ENGINE_MATH_VECTOR2_H

#include <iostream>
#include "FloatMath.h"

namespace Engine
{
	class Vector3;

	class Vector2
	{
		float m_x, m_y;
	public:
		static const Vector2 zero;		//(0, 0)
		static const Vector2 one;		//(1, 1)
		static const Vector2 up;		//(0, 1)
		static const Vector2 down;		//(0, -1)
		static const Vector2 left;		//(-1, 0)
		static const Vector2 right;		//(1, 0)

		inline Vector2();
		inline Vector2(float i_x, float i_y);
		Vector2(const Vector3 &i_vec);
		inline ~Vector2();

		inline void set(float i_x, float i_y);

		inline float angle(const Vector2 &rhs) const;
		inline float dot(const Vector2 &rhs) const;
		inline float distance(const Vector2 &rhs) const;

		inline float magnitude() const;
		inline float sq_magnitude() const;
		inline Vector2 normalized() const;

		inline Vector2 Reflect(const Vector2& i_normal);
		inline Vector2 AbsoluteValues();

		//setters
		inline void x(float i_x) { m_x = i_x; }
		inline void y(float i_y) { m_y = i_y; }

		//getters
		inline float x(void) const { return m_x; }
		inline float y(void) const { return m_y; }

		inline Vector2& operator+=(const Vector2& rhs);
		inline Vector2& operator-=(const Vector2& rhs);
		inline Vector2& operator*=(const float &rhs);
		inline Vector2& operator/=(const float &rhs);
	};

	inline Vector2 operator+(const Vector2 &i_lhs, const Vector2 &i_rhs);
	inline Vector2 operator-(const Vector2 &i_lhs, const Vector2 &i_rhs);

	inline Vector2 operator*(const Vector2 &i_lhs, const float &i_rhs);
	inline Vector2 operator*(const float &i_lhs, const Vector2 &i_rhs);

	inline Vector2 operator/(const Vector2 &i_lhs, const float &i_rhs);
	inline Vector2 operator/(const float &i_lhs, const Vector2 &i_rhs);

	inline bool operator==(const Vector2 &i_lhs, const Vector2 &i_rhs);
	inline bool operator!=(const Vector2 &i_lhs, const Vector2 &i_rhs);
	inline std::ostream& operator<< (std::ostream &out, Vector2 &vec);		//streams vector as (x, y)

	#include "Vector2.inl"
}

#endif //_ENGINE_MATH_VECTOR2_H