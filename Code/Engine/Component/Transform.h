#ifndef _ENGINE_COMPONENT_TRANSFORM_H
#define _ENGINE_COMPONENT_TRANSFORM_H

#include "../Core/Vector3.h"
#include "../Core/Quaternion.h"
#include "../Core/Matrix4x4.h"

namespace Lame
{
	class Transform
	{
	public:
		Transform() {}
		Transform(const Vector3& i_pos, const Quaternion& i_rot, const Vector3& i_scale) :position_(i_pos), rotation_(i_rot), scale_(i_scale) {}

		static inline Transform CreateDefault() { return Transform(Lame::Vector3::zero, Lame::Quaternion::identity, Lame::Vector3::one); }

		inline Vector3 position() const { return position_; }
		inline void position(const Vector3& i_pos) { position_ = i_pos; }

		inline Quaternion rotation() const { return rotation_; }
		inline void rotation(const Quaternion& i_rotation) { rotation_ = i_rotation; }

		inline Vector3 scale() const { return scale_; }
		inline void scale(const Vector3& i_scale) { scale_ = i_scale; }

		inline Matrix4x4 LocalToWorld() const { return Matrix4x4::CreateTransformation(position(), rotation(), scale()); }
		inline Matrix4x4 WorldToLocal() const { return LocalToWorld().Inverse(); }

		void Move(const Lame::Vector3& i_movement);
		void Rotate(const Lame::Quaternion& i_rotation);

	private:
		Vector3 position_;
		Quaternion rotation_;
		Vector3 scale_;
	};
}

#endif //_ENGINE_COMPONENT_TRANSFORM_H
