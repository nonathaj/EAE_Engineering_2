
inline Lame::Vector2::Vector2()
{
}

inline Lame::Vector2::Vector2(float i_x, float i_y)
	:m_x(i_x), m_y(i_y)
{
}

inline Lame::Vector2::~Vector2()
{
}

inline void Lame::Vector2::set(float i_x, float i_y)
{
	m_x = i_x;
	m_y = i_y;
}

inline float Lame::Vector2::angle(const Vector2 &rhs) const
{
	return acos(dot(rhs));
}

inline float Lame::Vector2::dot(const Vector2 &rhs) const
{
	return x() * rhs.x() + y() * rhs.y();
}

inline float Lame::Vector2::distance(const Vector2 &rhs) const
{
	return (*this - rhs).magnitude();
}

inline float Lame::Vector2::magnitude() const
{
	return sqrt(sq_magnitude());
}

inline float Lame::Vector2::sq_magnitude() const
{
	return x() * x() + y() * y();
}

inline Lame::Vector2 Lame::Vector2::normalized() const
{
	if (*this == Vector2::zero)
		return Vector2::zero;

	float mag = magnitude();
	return Vector2(x() / mag, y() / mag);
}


inline Lame::Vector2 Lame::Vector2::Reflect(const Vector2& i_normal)
{
	Lame::Vector2 norm = i_normal.normalized();		//ensure that normal is length 1
	return *this - 2.0f * this->dot(norm) * norm;
}

inline Lame::Vector2 Lame::Vector2::AbsoluteValues()
{
	return Lame::Vector2(fabs(x()), fabs(y()));
}

inline Lame::Vector2 operator+(const Lame::Vector2 &i_lhs, const Lame::Vector2 &i_rhs)
{
	return Lame::Vector2(i_lhs.x() + i_rhs.x(), i_lhs.y() + i_rhs.y());
}

inline Lame::Vector2& Lame::Vector2::operator+=(const Lame::Vector2& rhs)
{
	*this = *this + rhs;
	return *this;
}

inline Lame::Vector2 operator-(const Lame::Vector2 &i_lhs, const Lame::Vector2 &i_rhs)
{
	return Lame::Vector2(i_lhs.x() - i_rhs.x(), i_lhs.y() - i_rhs.y());
}

inline Lame::Vector2& Lame::Vector2::operator-=(const Lame::Vector2& rhs)
{
	*this = *this - rhs;
	return *this;
}

inline Lame::Vector2& Lame::Vector2::operator+=(const float &rhs)
{
	*this = *this + rhs;
	return *this;
}

inline Lame::Vector2& Lame::Vector2::operator-=(const float &rhs)
{
	*this = *this - rhs;
	return *this;
}

inline Lame::Vector2 operator*(const Lame::Vector2 &i_lhs, const float &i_rhs)
{
	return Lame::Vector2(i_lhs.x() * i_rhs, i_lhs.y() * i_rhs);
}

inline Lame::Vector2 operator*(const float &i_lhs, const Lame::Vector2 &i_rhs)
{
	return i_rhs * i_lhs;
}

inline Lame::Vector2& Lame::Vector2::operator*=(const float& rhs)
{
	*this = *this * rhs;
	return *this;
}

inline Lame::Vector2 operator/(const Lame::Vector2 &i_lhs, const float &i_rhs)
{
	return Lame::Vector2(i_lhs.x() / i_rhs, i_lhs.y() / i_rhs);
}

inline Lame::Vector2& Lame::Vector2::operator/=(const float& rhs)
{
	*this = *this / rhs;
	return *this;
}

inline bool operator==(const Lame::Vector2 &i_lhs, const Lame::Vector2 &i_rhs)
{
	return Lame::Math::Float::AlmostEqual(i_lhs.x(), i_rhs.x()) && Lame::Math::Float::AlmostEqual(i_lhs.y(), i_rhs.y());
}

inline bool operator!=(const Lame::Vector2 &i_lhs, const Lame::Vector2 &i_rhs)
{
	return !Lame::Math::Float::AlmostEqual(i_lhs.x(), i_rhs.x()) || !Lame::Math::Float::AlmostEqual(i_lhs.y(), i_rhs.y());
}

inline std::ostream& operator<<(std::ostream &out, Lame::Vector2 &vec)
{
	out << "(" << vec.x() << ", " << vec.y() << ")";
	return out;
}

inline Lame::Vector2 operator+(const Lame::Vector2 &i_lhs, const float i_rhs)
{
	return Lame::Vector2(i_lhs.x() + i_rhs, i_lhs.y() + i_rhs);
}

inline Lame::Vector2 operator+(const float i_lhs, const Lame::Vector2 &i_rhs)
{
	return Lame::Vector2(i_rhs.x() + i_lhs, i_rhs.y() + i_lhs);
}

inline Lame::Vector2 operator-(const Lame::Vector2 &i_lhs, const float i_rhs)
{
	return Lame::Vector2(i_lhs.x() - i_rhs, i_lhs.y() - i_rhs);
}

inline Lame::Vector2 operator-(const float i_lhs, const Lame::Vector2 & i_rhs)
{
	return Lame::Vector2(i_rhs.x() - i_lhs, i_rhs.y() - i_lhs);
}
