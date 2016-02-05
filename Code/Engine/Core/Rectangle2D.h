#ifndef _ENGINE_CORE_RECTANGLE_H
#define _ENGINE_CORE_RECTANGLE_H

namespace Lame
{
	class Vector2;

	class Rectangle2D
	{
	public:
		inline Rectangle2D() {}
		Rectangle2D(const float i_left, const float i_right, const float i_top, const float i_bottom);
		Rectangle2D(const Vector2& i_center, const Vector2& i_extends);

		inline static Rectangle2D CreateTLNormalized() { return Rectangle2D(0.0f, 1.0f, 0.0f, 1.0f); }
		inline static Rectangle2D CreateBLNormalized() { return Rectangle2D(0.0f, 1.0f, 1.0f, 0.0f); }

		inline void left(const float i_left) { left_ = i_left; }
		inline void right(const float i_right) { right_ = i_right; }
		inline void top(const float i_top) { top_ = i_top; }
		inline void bottom(const float i_bottom) { bottom_ = i_bottom; }

		inline float left() const { return left_; }
		inline float right() const { return right_; }
		inline float top() const { return top_; }
		inline float bottom() const { return bottom_; }

		inline float x() const { return (right() + left()) / 2.0f; }
		inline float y() const { return (top() + bottom()) / 2.0f; }
		inline float height() const { return top() - bottom(); }
		inline float width() const { return right() - left(); }

		Vector2 center() const;
		Vector2 extends() const;

	private:
		float left_, right_, top_, bottom_;
	};
}

#endif _ENGINE_CORE_RECTANGLE_H