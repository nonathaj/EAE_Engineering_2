// Header Files
//=============

#include "Quaternion.h"

#include <cassert>
#include <cmath>
#include <limits>

#include "Vector3.h"

namespace Engine
{
	const Quaternion Quaternion::identity(0.0f, 0.0f, 0.0f, 1.0f);

	Quaternion::Quaternion()
	{
	}

	Quaternion::Quaternion(const Vector3& i_complex, const float i_real) :
		m_w(i_real), m_x(i_complex.x()), m_y(i_complex.y()), m_z(i_complex.z())
	{
	}

	Quaternion::Quaternion(const float i_x, const float i_y, const float i_z, const float i_w) :
		m_w(i_w), m_x(i_x), m_y(i_y), m_z(i_z)
	{
	}

	Quaternion::Quaternion(const Vector3& i_euler_angles)
	{
		Vector3 euler_radians = Vector3(
			static_cast<float>(Engine::Math::ToRadians(i_euler_angles.x())),
			static_cast<float>(Engine::Math::ToRadians(i_euler_angles.y())),
			static_cast<float>(Engine::Math::ToRadians(i_euler_angles.z()))
		);

		const float c1 = cos(euler_radians.z() * 0.5f);
		const float c2 = cos(euler_radians.y() * 0.5f);
		const float c3 = cos(euler_radians.x() * 0.5f);
		const float s1 = sin(euler_radians.z() * 0.5f);
		const float s2 = sin(euler_radians.y() * 0.5f);
		const float s3 = sin(euler_radians.x() * 0.5f);

		m_x = c1 * c2 * s3 - s1 * s2 * c3;
		m_y = c1 * s2 * c3 + s1 * c2 * s3;
		m_z = s1 * c2 * c3 - c1 * s2 * s3;
		m_w = c1 * c2 * c3 + s1 * s2 * s3;
	}

	Quaternion::Quaternion(const float i_angleInRadians, const Vector3& i_axisOfRotation_normalized)
	{
		const float theta_half = i_angleInRadians * 0.5f;
		m_w = std::cos(theta_half);
		const float sin_theta_half = std::sin(theta_half);
		m_x = i_axisOfRotation_normalized.x() * sin_theta_half;
		m_y = i_axisOfRotation_normalized.y() * sin_theta_half;
		m_z = i_axisOfRotation_normalized.z() * sin_theta_half;
	}

	void Quaternion::invert()
	{
		m_x = -m_x;
		m_y = -m_y;
		m_z = -m_z;
	}

	Quaternion Quaternion::inverse() const
	{
		return Quaternion(-m_x, -m_y, -m_z, m_w);
	}

	Quaternion& Quaternion::normalize()
	{
		const float length = magnitude();
		assert(length > std::numeric_limits<float>::epsilon());
		const float length_reciprocal = 1.0f / length;
		m_w *= length_reciprocal;
		m_x *= length_reciprocal;
		m_y *= length_reciprocal;
		m_z *= length_reciprocal;
		return *this;
	}

	Quaternion Quaternion::normalized() const
	{
		const float length = magnitude();
		assert(length > std::numeric_limits<float>::epsilon());
		const float length_reciprocal = 1.0f / length;
		return Quaternion(m_x * length_reciprocal, m_y * length_reciprocal, m_z * length_reciprocal, m_w * length_reciprocal);
	}

	Quaternion Quaternion::conjugate() const
	{
		return Quaternion(-complex(), w());
	}

	Vector3 Quaternion::complex() const
	{
		return Vector3(x(), y(), z());
	}

	Vector3 Quaternion::euler() const
	{
		Vector3 euler;

		// quick conversion to Euler angles to give tilt to user
		const float sqw = w() * w();
		const float sqx = x() * x();
		const float sqy = y() * y();
		const float sqz = z() * z();

		euler.y( asin(2.0f * (w() * y() - x() * z())) );
		if ((M_PI / 2) - fabs(euler.y()) > std::numeric_limits<float>::epsilon()) {
			euler.z( atan2(2.0f * (x() * y() + w() * z()), sqx - sqy - sqz + sqw) );
			euler.x( atan2(2.0f * (w() * x() + y() * z()), sqw - sqx - sqy + sqz) );
		}
		else {
			// compute heading from local 'down' vector
			euler.z( atan2(2 * y() * z() - 2 * x() * w(), 2 * x() * z() + 2 * y() * w()) );
			euler.x(0.0f);

			// If facing down, reverse yaw
			if (euler.y() < 0)
				euler.z(static_cast<float>(M_PI) - euler.z());
		}

		//convert to degrees
		euler = Vector3(
			static_cast<float>(Engine::Math::ToDegrees(euler.x())),
			static_cast<float>(Engine::Math::ToDegrees(euler.y())),
			static_cast<float>(Engine::Math::ToDegrees(euler.z()))
		);

		return euler;
	}

	float Quaternion::dot(const Quaternion& i_rhs) const
	{
		return (w() * i_rhs.w()) + (x() * i_rhs.x()) + (y() * i_rhs.y()) + (z() * i_rhs.z());
	}

	float Quaternion::sq_magnitude(void) const
	{
		return m_w * m_w + m_x * m_x + m_y * m_y + m_z * m_z;
	}

	float Quaternion::magnitude(void) const
	{
		return std::sqrt(sq_magnitude());
	}

	// Concatenation
	Quaternion operator*(const Quaternion& i_lhs, const Quaternion& i_rhs)
	{
		return Quaternion(
			(i_lhs.w() * i_rhs.x()) + (i_lhs.x() * i_rhs.w()) + ((i_lhs.y() * i_rhs.z()) - (i_lhs.z() * i_rhs.y())),
			(i_lhs.w() * i_rhs.y()) + (i_lhs.y() * i_rhs.w()) + ((i_lhs.z() * i_rhs.x()) - (i_lhs.x() * i_rhs.z())),
			(i_lhs.w() * i_rhs.z()) + (i_lhs.z() * i_rhs.w()) + ((i_lhs.x() * i_rhs.y()) - (i_lhs.y() * i_rhs.x())),
			(i_lhs.w() * i_rhs.w()) - ((i_lhs.x() * i_rhs.x()) + (i_lhs.y() * i_rhs.y()) + (i_lhs.z() * i_rhs.z())));
	}

	Vector3 operator*(const Quaternion& i_lhs, const Vector3& i_rhs)
	{
		//https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
		Vector3 u(i_lhs.x(), i_lhs.y(), i_lhs.z());
		return 2.0f * u.dot(i_rhs) * u
			+ (i_lhs.w() * i_lhs.w() - u.dot(u)) * i_rhs
			+ 2.0f * i_lhs.w() * u.cross(i_rhs);
	}
}
