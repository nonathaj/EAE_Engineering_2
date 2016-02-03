
inline Engine::Vector2::Vector2()
{
}

inline Engine::Vector2::Vector2(float i_x, float i_y)
	:m_x(i_x), m_y(i_y)
{
}

inline Engine::Vector2::~Vector2()
{
}

inline void Engine::Vector2::set(float i_x, float i_y)
{
	m_x = i_x;
	m_y = i_y;
}

inline float Engine::Vector2::angle(const Vector2 &rhs) const
{
	return acos(dot(rhs));
}

inline float Engine::Vector2::dot(const Vector2 &rhs) const
{
	return x() * rhs.x() + y() * rhs.y();
}

inline float Engine::Vector2::distance(const Vector2 &rhs) const
{
	return (*this - rhs).magnitude();
}

inline float Engine::Vector2::magnitude() const
{
	return sqrt(sq_magnitude());
}

inline float Engine::Vector2::sq_magnitude() const
{
	return x() * x() + y() * y();
}

inline Engine::Vector2 Engine::Vector2::normalized() const
{
	if (*this == Vector2::zero)
		return Vector2::zero;

	float mag = magnitude();
	return Vector2(x() / mag, y() / mag);
}


inline Engine::Vector2 Engine::Vector2::Reflect(const Vector2& i_normal)
{
	Engine::Vector2 norm = i_normal.normalized();		//ensure that normal is length 1
	return *this - 2.0f * this->dot(norm) * norm;
}

inline Engine::Vector2 Engine::Vector2::AbsoluteValues()
{
	return Engine::Vector2(fabs(x()), fabs(y()));
}

inline Engine::Vector2 operator+(const Engine::Vector2 &i_lhs, const Engine::Vector2 &i_rhs)
{
	return Engine::Vector2(i_lhs.x() + i_rhs.x(), i_lhs.y() + i_rhs.y());
}

inline Engine::Vector2& Engine::Vector2::operator+=(const Engine::Vector2& rhs)
{
	*this = *this + rhs;
	return *this;
}

inline Engine::Vector2 operator-(const Engine::Vector2 &i_lhs, const Engine::Vector2 &i_rhs)
{
	return Engine::Vector2(i_lhs.x() - i_rhs.x(), i_lhs.y() - i_rhs.y());
}

inline Engine::Vector2& Engine::Vector2::operator-=(const Engine::Vector2& rhs)
{
	*this = *this - rhs;
	return *this;
}

inline Engine::Vector2& Engine::Vector2::operator+=(const float &rhs)
{
	*this = *this + rhs;
	return *this;
}

inline Engine::Vector2& Engine::Vector2::operator-=(const float &rhs)
{
	*this = *this - rhs;
	return *this;
}

inline Engine::Vector2 operator*(const Engine::Vector2 &i_lhs, const float &i_rhs)
{
	return Engine::Vector2(i_lhs.x() * i_rhs, i_lhs.y() * i_rhs);
}

inline Engine::Vector2 operator*(const float &i_lhs, const Engine::Vector2 &i_rhs)
{
	return i_rhs * i_lhs;
}

inline Engine::Vector2& Engine::Vector2::operator*=(const float& rhs)
{
	*this = *this * rhs;
	return *this;
}

inline Engine::Vector2 operator/(const Engine::Vector2 &i_lhs, const float &i_rhs)
{
	return Engine::Vector2(i_lhs.x() / i_rhs, i_lhs.y() / i_rhs);
}

inline Engine::Vector2& Engine::Vector2::operator/=(const float& rhs)
{
	*this = *this / rhs;
	return *this;
}

inline bool operator==(const Engine::Vector2 &i_lhs, const Engine::Vector2 &i_rhs)
{
	return Engine::Math::Float::AlmostEqual(i_lhs.x(), i_rhs.x()) && Engine::Math::Float::AlmostEqual(i_lhs.y(), i_rhs.y());
}

inline bool operator!=(const Engine::Vector2 &i_lhs, const Engine::Vector2 &i_rhs)
{
	return !Engine::Math::Float::AlmostEqual(i_lhs.x(), i_rhs.x()) || !Engine::Math::Float::AlmostEqual(i_lhs.y(), i_rhs.y());
}

inline std::ostream& operator<<(std::ostream &out, Engine::Vector2 &vec)
{
	out << "(" << vec.x() << ", " << vec.y() << ")";
	return out;
}

inline Engine::Vector2 operator+(const Engine::Vector2 &i_lhs, const float i_rhs)
{
	return Engine::Vector2(i_lhs.x() + i_rhs, i_lhs.y() + i_rhs);
}

inline Engine::Vector2 operator+(const float i_lhs, const Engine::Vector2 &i_rhs)
{
	return Engine::Vector2(i_rhs.x() + i_lhs, i_rhs.y() + i_lhs);
}

inline Engine::Vector2 operator-(const Engine::Vector2 &i_lhs, const float i_rhs)
{
	return Engine::Vector2(i_lhs.x() - i_rhs, i_lhs.y() - i_rhs);
}

inline Engine::Vector2 operator-(const float i_lhs, const Engine::Vector2 & i_rhs)
{
	return Engine::Vector2(i_rhs.x() - i_lhs, i_rhs.y() - i_lhs);
}
