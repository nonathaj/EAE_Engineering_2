#ifndef _LAME_PHYSICS_H
#define _LAME_PHYSICS_H

#include <memory>

#include "../Core/Singleton.h"
#include "../Core/Vector3.h"

namespace Lame
{
	class Physics3DComponent;

	class Physics
	{
	public:
		bool Setup();

		void Tick(float deltaTime);

		Vector3 gravity() const { return gravity_; }
		void gravity(const Vector3& i_gravity) { gravity_ = i_gravity; }

		float fixed_timestep() const { return fixed_timestep_; }
		void fixed_timestep(const float i_fixed_timestep) { fixed_timestep_ = i_fixed_timestep; }

		void Predict(std::shared_ptr<Physics3DComponent> i_comp, const float i_delta_time, Vector3& io_postion, Vector3& io_velocity) const;

		static void Predict(const float i_delta_time, Vector3& io_postion, Vector3& io_velocity, const Vector3 i_acceleration);
	private:
		Physics();

		void Simulate();

		float current_frame_time_remaining;

		float fixed_timestep_;
		Vector3 gravity_;

		friend Lame::Singleton<Lame::Physics>;
	};
}

typedef Lame::Singleton<Lame::Physics> LamePhysics;

#endif //_LAME_PHYSICS_H
