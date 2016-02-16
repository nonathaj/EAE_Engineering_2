
#include "Physics.h"

#include "Physics3DComponent.h"
#include "../Component/GameObject.h"
#include "../Component/Transform.h"

namespace Lame
{
	Physics::Physics() :
		gravity_(0, -9.81f, 0),
		fixed_timestep_(0.02f),
		current_frame_time_remaining(0.0f)
	{
	}

	bool Physics::Setup()
	{
		return true;
	}

	void Physics::Tick(float deltaTime)
	{
		current_frame_time_remaining += deltaTime;
		while (current_frame_time_remaining >= fixed_timestep_)
		{
			current_frame_time_remaining -= fixed_timestep_;
			Simulate();
		}
	}

	void Physics::Simulate()
	{

	}

	void Physics::Predict(std::shared_ptr<Physics3DComponent> i_comp, const float i_delta_time, Vector3& io_postion, Vector3& io_velocity) const
	{
		io_postion = i_comp->gameObject()->transform().position();
		io_velocity = i_comp->velocity();
		const Vector3 acceleration = i_comp->constant_acceleration() + gravity() * i_comp->gravity_multiplier();
		Predict(i_delta_time, io_postion, io_velocity, acceleration);
	}

	void Physics::Predict(const float i_delta_time, Vector3& io_position, Vector3& io_velocity, const Vector3 i_acceleration)
	{
		io_position += io_velocity + 0.5f * i_acceleration * i_delta_time * i_delta_time;
		io_velocity += i_acceleration * i_delta_time;
	}
}
