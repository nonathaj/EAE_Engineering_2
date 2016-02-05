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
		Transform(const Lame::Vector3& i_pos, const Lame::Quaternion& i_rot) :position_(i_pos), rotation_(i_rot) {}

		static inline Transform CreateDefault() { return Transform(Lame::Vector3::zero, Lame::Quaternion::identity); }

		inline Lame::Vector3 position() const { return position_; }
		inline void position(const Lame::Vector3& i_pos) { position_ = i_pos; }

		inline Lame::Quaternion rotation() const { return rotation_; }
		inline void rotation(const Lame::Quaternion& i_rotation) { rotation_ = i_rotation; }

		inline Lame::Matrix4x4 LocalToWorld() const { return Lame::Matrix4x4::CreateTransformation(position(), rotation()); }
		inline Lame::Matrix4x4 WorldToLocal() const { return LocalToWorld().Inverse(); }

		void Move(const Lame::Vector3& i_movement);
		void Rotate(const Lame::Quaternion& i_rotation);

	private:
		Lame::Vector3 position_;
		Lame::Quaternion rotation_;
	};
}

#endif //_ENGINE_COMPONENT_TRANSFORM_H
