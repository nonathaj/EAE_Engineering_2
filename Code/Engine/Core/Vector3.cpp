
#include "Vector3.h"
#include "Vector2.h"
#include "FloatMath.h"

#include <sstream>

const Lame::Vector3 Lame::Vector3::zero = Vector3(0, 0, 0);
const Lame::Vector3 Lame::Vector3::one = Vector3(1, 1, 1);
const Lame::Vector3 Lame::Vector3::up = Vector3(0, 1, 0);
const Lame::Vector3 Lame::Vector3::down = Vector3(0, -1, 0);
const Lame::Vector3 Lame::Vector3::left = Vector3(-1, 0, 0);
const Lame::Vector3 Lame::Vector3::right = Vector3(1, 0, 0);
const Lame::Vector3 Lame::Vector3::forward = Vector3(0, 0, -1);
const Lame::Vector3 Lame::Vector3::back = Vector3(0, 0, 1);

Lame::Vector3::Vector3(const Vector2 &i_vec, float i_z)
	:m_x(i_vec.x()), m_y(i_vec.y()), m_z(i_z)
{
}

std::string Lame::Vector3::to_string() const
{
	std::stringstream str;
	str << "(" << x() << ", " << y() << ", " << z() << ")";
	return str.str();
}

bool Lame::Vector3::AnyNaN() const
{
	return Lame::Math::Float::IsNaN(x()) || Lame::Math::Float::IsNaN(y()) || Lame::Math::Float::IsNaN(z());
}

bool Lame::Vector3::AllNaN() const
{
	return Lame::Math::Float::IsNaN(x()) && Lame::Math::Float::IsNaN(y()) && Lame::Math::Float::IsNaN(z());
}
