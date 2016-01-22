#ifndef _ENGINE_MATH_CQUATERNION_H
#define _ENGINE_MATH_CQUATERNION_H

//Based on https://cs.stanford.edu/~acoates/quaternion.h
namespace Engine
{
	class Vector3;

	class Quaternion
	{
	public:
		static const Quaternion identity;
		static const Quaternion zero;

		Quaternion();
		Quaternion(const Vector3& i_complex, const float i_real);
		Quaternion(const float i_w, const float i_x, const float i_y, const float i_z);
		Quaternion(const float i_angleInRadians, const Vector3& i_axisOfRotation_normalized);

		static Quaternion Euler(const float i_euler_x_deg, const float i_euler_y_deg, const float i_euler_z_deg);
		static Quaternion Euler(const Vector3& i_euler_angles);
		static Quaternion LookRotation(const Vector3& i_forward_direction, const Vector3& i_up_direction);
		
		void invert();
		Quaternion inverse() const;

		Quaternion& normalize();
		Quaternion normalized() const;
		float sq_magnitude(void) const;
		float magnitude(void) const;

		float dot(const Quaternion& i_rhs) const;
		Quaternion conjugate() const;
		Vector3 complex() const;
		Vector3 euler() const;

		inline float x() const { return m_x; }
		inline float y() const { return m_y; }
		inline float z() const { return m_z; }
		inline float w() const { return m_w; }

		inline void x(float i_x) { m_x = i_x; }
		inline void y(float i_y) { m_y = i_y; }
		inline void z(float i_z) { m_z = i_z; }
		inline void w(float i_w) { m_w = i_w; }

	private:
		float m_w, m_x, m_y, m_z;
	};

	Quaternion operator *(const Quaternion& i_lhs, const Quaternion& i_rhs);	// Concatenation
	Vector3 operator*(const Quaternion& i_lhs, const Vector3& i_rhs);			// Rotation
}

#endif	// _ENGINE_MATH_CQUATERNION_H
