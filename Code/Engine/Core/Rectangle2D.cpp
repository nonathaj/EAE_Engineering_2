
#include "Rectangle2D.h"
#include "Vector2.h"

namespace Lame
{
	Rectangle2D::Rectangle2D(const float i_left, const float i_right, const float i_top, const float i_bottom) :
		left_(i_left),
		right_(i_right),
		top_(i_top),
		bottom_(i_bottom)
	{
	}

	Rectangle2D::Rectangle2D(const Vector2& i_center, const Vector2& i_extends) :
		left_(i_center.x() - i_extends.x() / 2),
		right_(i_center.x() + i_extends.x() / 2),
		top_(i_center.y() + i_extends.y() / 2),
		bottom_(i_center.y() - i_extends.y() / 2)
	{

	}

	Vector2 Rectangle2D::center() const
	{
		return Lame::Vector2(x(), y());
	}

	Vector2 Rectangle2D::extends() const
	{
		return Lame::Vector2(width(), height());
	}
}