
inline Engine::Vector3::Vector3()
{
}

inline Engine::Vector3::Vector3(const Vector3 &i_vec)
	:m_x(i_vec.m_x), m_y(i_vec.m_y), m_z(i_vec.m_z)
{
}

inline Engine::Vector3::Vector3(float i_x, float i_y, float i_z)
	:m_x(i_x), m_y(i_y), m_z(i_z)
{
}

inline Engine::Vector3::~Vector3()
{
}

inline void Engine::Vector3::set(float i_x, float i_y, float i_z)
{
	m_x = i_x;
	m_y = i_y;
	m_z = i_z;
}

inline float Engine::Vector3::angle(const Vector3 &rhs) const
{
	return acos( dot(rhs) );
}

inline float Engine::Vector3::dot(const Vector3 &rhs) const
{
	return x() * rhs.x() + y() * rhs.y() + z() * rhs.z();
}

inline Engine::Vector3 Engine::Vector3::cross(const Vector3 &rhs) const
{
	Vector3 ans;
	ans.x( y() * rhs.z() - z() * rhs.y() );
	ans.y( z() * rhs.x() - x() * rhs.z() );
	ans.z( x() * rhs.y() - y() * rhs.x() );
	return ans;
}

inline float Engine::Vector3::distance(const Vector3 &rhs) const
{
	return (*this - rhs).magnitude();
}

inline float Engine::Vector3::magnitude(void) const
{
	return sqrt(sq_magnitude());
}

inline float Engine::Vector3::sq_magnitude(void) const
{
	return x() * x() + y() * y() + z() * z();
}

inline Engine::Vector3 Engine::Vector3::normalized(void) const
{
	if (*this == Vector3::zero)
		return Vector3::zero;

	float mag = magnitude();
	return Vector3(x() / mag, y() / mag, z() / mag);
}

inline Engine::Vector3& Engine::Vector3::normalize(void)
{
	float mag = magnitude();
	if (mag != 0.0f)
	{
		x(x() / mag);
		y(y() / mag);
		z(z() / mag);
	}
	return *this;
}

inline Engine::Vector3 Engine::operator+(const Engine::Vector3 &i_lhs, const Engine::Vector3 &i_rhs)
{
	return Engine::Vector3(i_lhs.x() + i_rhs.x(), i_lhs.y() + i_rhs.y(), i_lhs.z() + i_rhs.z());
}

inline Engine::Vector3& Engine::Vector3::operator+=(const Engine::Vector3& i_rhs)
{
	*this = *this + i_rhs;
	return *this;
}

inline Engine::Vector3 Engine::operator-(const Engine::Vector3 &i_lhs, const Engine::Vector3 &i_rhs)
{
	return Engine::Vector3(i_lhs.x() - i_rhs.x(), i_lhs.y() - i_rhs.y(), i_lhs.z() - i_rhs.z());
}

inline Engine::Vector3& Engine::Vector3::operator-=(const Engine::Vector3& i_rhs)
{
	*this = *this - i_rhs;
	return *this;
}

inline Engine::Vector3 Engine::operator-(const Engine::Vector3 &i_rhs)
{
	return Engine::Vector3(-i_rhs.x(), -i_rhs.y(), -i_rhs.z());
}

inline Engine::Vector3 Engine::operator*(const float &i_lhs, const Engine::Vector3 &i_rhs)
{
	return i_rhs * i_lhs;
}

inline Engine::Vector3 Engine::operator*(const double &i_lhs, const Engine::Vector3 &i_rhs)
{
	return i_rhs * i_lhs;
}

inline Engine::Vector3 Engine::operator*(const Engine::Vector3 &i_lhs, const float &i_rhs)
{
	return Engine::Vector3(i_lhs.x() * i_rhs, i_lhs.y() * i_rhs, i_lhs.z() * i_rhs);
}

inline Engine::Vector3 Engine::operator*(const Engine::Vector3 &i_lhs, const double &i_rhs)
{
	return Engine::Vector3(static_cast<float>(static_cast<double>(i_lhs.x()) * i_rhs), static_cast<float>(static_cast<double>(i_lhs.y()) * i_rhs), static_cast<float>(static_cast<double>(i_lhs.z()) * i_rhs));
}

inline Engine::Vector3& Engine::Vector3::operator*=(const float& rhs)
{
	*this = *this * rhs;
	return *this;
}

inline Engine::Vector3& Engine::Vector3::operator*=(const double& rhs)
{
	*this = *this * rhs;
	return *this;
}

inline Engine::Vector3 Engine::operator/(const Engine::Vector3 &i_lhs, const float &i_rhs)
{
	return Engine::Vector3(i_lhs.x() / i_rhs, i_lhs.y() / i_rhs, i_lhs.z() / i_rhs);
}

inline Engine::Vector3 Engine::operator/(const Engine::Vector3 &i_lhs, const double &i_rhs)
{
	return Engine::Vector3(static_cast<float>(static_cast<double>(i_lhs.x()) / i_rhs), static_cast<float>(static_cast<double>(i_lhs.y()) / i_rhs), static_cast<float>(static_cast<double>(i_lhs.z()) / i_rhs));
}

inline Engine::Vector3& Engine::Vector3::operator/=(const float& rhs)
{
	*this = *this / rhs;
	return *this;
}

inline Engine::Vector3& Engine::Vector3::operator/=(const double& rhs)
{
	*this = *this / rhs;
	return *this;
}

inline bool Engine::operator==(const Engine::Vector3 &i_lhs, const Engine::Vector3 &i_rhs)
{
	return Engine::Math::Float::AlmostEqual(i_lhs.x(), i_rhs.x()) && Engine::Math::Float::AlmostEqual(i_lhs.y(), i_rhs.y()) && Engine::Math::Float::AlmostEqual(i_lhs.z(), i_rhs.z());
}

inline bool Engine::operator!=(const Engine::Vector3 &i_lhs, const Engine::Vector3 &i_rhs)
{
	return !Engine::Math::Float::AlmostEqual(i_lhs.x(), i_rhs.x()) || !Engine::Math::Float::AlmostEqual(i_lhs.y(), i_rhs.y()) || !Engine::Math::Float::AlmostEqual(i_lhs.z(), i_rhs.z());
}

inline std::ostream& Engine::operator<<(std::ostream &out, Engine::Vector3 &vec)
{
	out << "(" << vec.x() << ", " << vec.y() << ", " << vec.z() << ")";
	return out;
}

inline Engine::Vector3& Engine::Vector3::operator=(const Engine::Vector3 &rhs)
{
	x(rhs.x());
	y(rhs.y());
	z(rhs.z());
	return *this;
}