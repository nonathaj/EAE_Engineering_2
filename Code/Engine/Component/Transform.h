#ifndef _ENGINE_COMPONENT_TRANSFORM_H
#define _ENGINE_COMPONENT_TRANSFORM_H

#include "../Core/Vector3.h"
#include "../Core/Quaternion.h"
#include "../Core/Matrix4x4.h"

namespace Engine
{
	class Transform
	{
	public:
		Transform() {}
		Transform(const Engine::Vector3& i_pos, const Engine::Quaternion& i_rot) :position_(i_pos), rotation_(i_rot) {}

		static inline Transform CreateDefault() { return Transform(Engine::Vector3::zero, Engine::Quaternion::identity); }

		inline Engine::Vector3 position() const { return position_; }
		inline void position(const Engine::Vector3& i_pos) { position_ = i_pos; }

		inline Engine::Quaternion rotation() const { return rotation_; }
		inline void rotation(const Engine::Quaternion& i_rotation) { rotation_ = i_rotation; }

		inline Engine::Matrix4x4 LocalToWorld() const { return Engine::Matrix4x4::CreateTransformation(position(), rotation()); }
		inline Engine::Matrix4x4 WorldToLocal() const { return LocalToWorld().Inverse(); }

		void Move(const Engine::Vector3& i_movement);
		void Rotate(const Engine::Quaternion& i_rotation);

	private:
		Engine::Vector3 position_;
		Engine::Quaternion rotation_;
	};
}

#endif //_ENGINE_COMPONENT_TRANSFORM_H
