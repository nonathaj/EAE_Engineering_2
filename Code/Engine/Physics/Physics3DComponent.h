#ifndef _LAME_PHYSICS3DCOMPONENT_H
#define _LAME_PHYSICS3DCOMPONENT_H

#include "../Component/IComponent.h"
#include "../Core/Vector3.h"

namespace Lame
{
	class Physics3DComponent : public IComponent
	{
	public:
		Physics3DComponent(std::weak_ptr<GameObject> go);

		Vector3 velocity() const { return velocity_; }
		void velocity(const Vector3& i_velocity) { velocity_ = i_velocity; }

		Vector3 constant_acceleration() const { return constant_acceleration_; }
		void constant_acceleration(const Vector3& i_const_accel) { constant_acceleration_ = i_const_accel; }

		float gravity_multiplier() const { return gravity_multiplier_; }
		void gravity_multiplier(const float i_grav_mul) { gravity_multiplier_ = i_grav_mul; }
	private:
		Vector3 velocity_;
		Vector3 constant_acceleration_;
		float gravity_multiplier_;
	};
}

#endif //_LAME_PHYSICS3DCOMPONENT_H
