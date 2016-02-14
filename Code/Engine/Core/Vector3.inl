
inline Lame::Vector3::Vector3()
{
}

inline Lame::Vector3::Vector3(const Vector3 &i_vec)
	:m_x(i_vec.m_x), m_y(i_vec.m_y), m_z(i_vec.m_z)
{
}

inline Lame::Vector3::Vector3(const float i_x, const float i_y, const float i_z)
	:m_x(i_x), m_y(i_y), m_z(i_z)
{
}

inline Lame::Vector3::~Vector3()
{
}

inline void Lame::Vector3::set(const float i_x, const float i_y, const float i_z)
{
	m_x = i_x;
	m_y = i_y;
	m_z = i_z;
}

inline float Lame::Vector3::angle(const Vector3 &rhs) const
{
	return acos( dot(rhs) );
}

inline float Lame::Vector3::dot(const Vector3 &rhs) const
{
	return x() * rhs.x() + y() * rhs.y() + z() * rhs.z();
}

inline Lame::Vector3 Lame::Vector3::cross(const Vector3 &rhs) const
{
	Vector3 ans;
	ans.x( y() * rhs.z() - z() * rhs.y() );
	ans.y( z() * rhs.x() - x() * rhs.z() );
	ans.z( x() * rhs.y() - y() * rhs.x() );
	return ans;
}

inline float Lame::Vector3::distance(const Vector3 &rhs) const
{
	return (*this - rhs).magnitude();
}

inline float Lame::Vector3::magnitude(void) const
{
	return sqrt(sq_magnitude());
}

inline float Lame::Vector3::sq_magnitude(void) const
{
	return this->dot(*this);
}

inline Lame::Vector3 Lame::Vector3::normalized(void) const
{
	if (*this == Vector3::zero)
		return Vector3::zero;

	float mag = magnitude();
	return Vector3(x() / mag, y() / mag, z() / mag);
}

inline Lame::Vector3& Lame::Vector3::normalize(void)
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

inline Lame::Vector3 Lame::Vector3::Reflect(const Vector3& i_normal) const
{
	Vector3 norm = i_normal.normalized();		//ensure that normal is length 1
	return *this - 2.0f * this->dot(norm) * norm;
}

inline Lame::Vector3 Lame::Vector3::ProjectOnPlane(const Vector3& i_normal) const
{
	//assumes the plane is through the origin
	return *this - i_normal.normalized() * this->dot(i_normal);
}

inline Lame::Vector3 Lame::Vector3::AbsoluteValues() const
{
	return Vector3(fabs(x()), fabs(y()), fabs(z()));
}

inline Lame::Vector3 Lame::Vector3::Cartesian(const Vector3 i_p1, const Vector3 i_p2, const Vector3 i_p3) const
{
	return i_p1 * x() + i_p2 * y() + i_p3 * z();
}

inline Lame::Vector3 Lame::Vector3::Barycentric(const Vector3 i_p1, const Vector3 i_p2, const Vector3 i_p3) const
{
	Vector3 v0 = i_p2 - i_p1;
	Vector3 v1 = i_p3 - i_p1;
	Vector3 v2 = *this - i_p1;
	float d00 = v0.dot(v0);
	float d01 = v0.dot(v1);
	float d11 = v1.dot(v1);
	float d20 = v2.dot(v0);
	float d21 = v2.dot(v1);
	float denom = d00 * d11 - d01 * d01;
	float v = (d11 * d20 - d01 * d21) / denom;
	float w = (d00 * d21 - d01 * d20) / denom;
	return Vector3(1.0f - v - w, v, w);
}

inline Lame::Vector3 Lame::operator+(const Lame::Vector3 &i_lhs, const Lame::Vector3 &i_rhs)
{
	return Lame::Vector3(i_lhs.x() + i_rhs.x(), i_lhs.y() + i_rhs.y(), i_lhs.z() + i_rhs.z());
}

inline Lame::Vector3& Lame::Vector3::operator+=(const Lame::Vector3& i_rhs)
{
	*this = *this + i_rhs;
	return *this;
}

inline Lame::Vector3 Lame::operator-(const Lame::Vector3 &i_lhs, const Lame::Vector3 &i_rhs)
{
	return Lame::Vector3(i_lhs.x() - i_rhs.x(), i_lhs.y() - i_rhs.y(), i_lhs.z() - i_rhs.z());
}

inline Lame::Vector3& Lame::Vector3::operator-=(const Lame::Vector3& i_rhs)
{
	*this = *this - i_rhs;
	return *this;
}

inline Lame::Vector3 Lame::operator-(const Lame::Vector3 &i_rhs)
{
	return Lame::Vector3(-i_rhs.x(), -i_rhs.y(), -i_rhs.z());
}

inline Lame::Vector3 Lame::operator*(const float &i_lhs, const Lame::Vector3 &i_rhs)
{
	return i_rhs * i_lhs;
}

inline Lame::Vector3 Lame::operator*(const Lame::Vector3 &i_lhs, const float &i_rhs)
{
	return Lame::Vector3(i_lhs.x() * i_rhs, i_lhs.y() * i_rhs, i_lhs.z() * i_rhs);
}

inline Lame::Vector3& Lame::Vector3::operator*=(const float& rhs)
{
	*this = *this * rhs;
	return *this;
}

inline Lame::Vector3 Lame::operator/(const Lame::Vector3 &i_lhs, const float &i_rhs)
{
	return Lame::Vector3(i_lhs.x() / i_rhs, i_lhs.y() / i_rhs, i_lhs.z() / i_rhs);
}

inline Lame::Vector3& Lame::Vector3::operator/=(const float& rhs)
{
	*this = *this / rhs;
	return *this;
}

inline bool Lame::operator==(const Lame::Vector3 &i_lhs, const Lame::Vector3 &i_rhs)
{
	return Lame::Math::Float::AlmostEqual(i_lhs.x(), i_rhs.x()) && Lame::Math::Float::AlmostEqual(i_lhs.y(), i_rhs.y()) && Lame::Math::Float::AlmostEqual(i_lhs.z(), i_rhs.z());
}

inline bool Lame::operator!=(const Lame::Vector3 &i_lhs, const Lame::Vector3 &i_rhs)
{
	return !Lame::Math::Float::AlmostEqual(i_lhs.x(), i_rhs.x()) || !Lame::Math::Float::AlmostEqual(i_lhs.y(), i_rhs.y()) || !Lame::Math::Float::AlmostEqual(i_lhs.z(), i_rhs.z());
}

inline std::ostream& Lame::operator<<(std::ostream &out, Lame::Vector3 &vec)
{
	out << "(" << vec.x() << ", " << vec.y() << ", " << vec.z() << ")";
	return out;
}

inline Lame::Vector3& Lame::Vector3::operator=(const Lame::Vector3 &rhs)
{
	x(rhs.x());
	y(rhs.y());
	z(rhs.z());
	return *this;
}
