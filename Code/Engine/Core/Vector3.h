#ifndef _ENGINE_MATH_VECTOR3_H
#define _ENGINE_MATH_VECTOR3_H

#include <iostream>
#include <math.h>
#include "FloatMath.h"

namespace Engine
{
	class Vector2;
	/*
		Basic Vector3 class for storing 3 variables and being able to do vector math on them.
	*/
	class Vector3
	{
	public:
		static const Vector3 zero;		//(0, 0, 0)
		static const Vector3 one;		//(1, 1, 1)
		static const Vector3 up;		//(0, 1, 0)
		static const Vector3 down;		//(0, -1, 0)
		static const Vector3 left;		//(-1, 0, 0)
		static const Vector3 right;		//(1, 0, 0)
		static const Vector3 forward;	//(0, 0, 1)
		static const Vector3 back;		//(0, 0, -1)

		inline Vector3();
		Vector3(const Vector2 &i_vec, float i_z = 0.0f);
		inline Vector3(const Vector3 &i_vec);
		inline Vector3(float i_x, float i_y, float i_z);
		inline ~Vector3();

		inline float angle(const Vector3 &rhs) const;			//returns the angle between vectors, in radians
		inline float dot(const Vector3 &rhs) const;				//returns the dot product of the 2 vectors
		inline Vector3 cross(const Vector3 &rhs) const;			//returns the cross product of the 2 vectors
		inline float distance(const Vector3 &rhs) const;		//returns the distance between the 2 vector positions

		inline float magnitude(void) const;						//returns the magnitude of the vector
		inline float sq_magnitude(void) const;					//returns the squared magnitude of the vectors
		inline Vector3 normalized(void) const;					//returns a normalized version of the vector (length 1)
		inline Vector3& normalize(void);						//normalizes this vector

		//setters
		inline void set(float i_x, float i_y, float i_z);
		inline void x(float i_x) { m_x = i_x; }
		inline void y(float i_y) { m_y = i_y; }
		inline void z(float i_z) { m_z = i_z; }

		//getters
		inline float x(void) const { return m_x; }
		inline float y(void) const { return m_y; }
		inline float z(void) const { return m_z; }

		//operators that alter the state of the class
		inline Vector3& operator+=(const Vector3& rhs);
		inline Vector3& operator-=(const Vector3& rhs);
		inline Vector3& operator*=(const float &rhs);
		inline Vector3& operator*=(const double &rhs);
		inline Vector3& operator/=(const float &rhs);
		inline Vector3& operator/=(const double &rhs);
		inline Vector3& operator=(const Vector3 &rhs);
	private:
		float m_x, m_y, m_z;
	};

	inline Vector3 operator+(const Vector3 &i_lhs, const Vector3 &i_rhs);
	inline Vector3 operator-(const Vector3 &i_lhs, const Vector3 &i_rhs);
	inline Vector3 operator-(const Vector3 &i_rhs);
	
	inline Vector3 operator*(const Vector3 &i_lhs, const float &i_rhs);
	inline Vector3 operator*(const float &i_lhs, const Vector3 &i_rhs);
	inline Vector3 operator*(const Vector3 &i_lhs, const double &i_rhs);
	inline Vector3 operator*(const double &i_lhs, const Vector3 &i_rhs);

	inline Vector3 operator/(const Vector3 &i_lhs, const float &i_rhs);
	inline Vector3 operator/(const Vector3 &i_lhs, const double &i_rhs);

	inline bool operator==(const Vector3 &i_lhs, const Vector3 &i_rhs);
	inline bool operator!=(const Vector3 &i_lhs, const Vector3 &i_rhs);

	inline std::ostream& operator<< (std::ostream &out, Vector3 &vec);		//streams vector as (x, y, z)
}
#include "Vector3.inl"

#endif //_ENGINE_MATH_VECTOR3_H
