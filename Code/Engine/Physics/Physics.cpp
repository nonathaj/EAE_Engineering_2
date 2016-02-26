
#include "Physics.h"

#include "Physics3DComponent.h"
#include "../Component/GameObject.h"
#include "../Component/Transform.h"
#include "CollisionMesh.h"
#include "../Component/World.h"

namespace Lame
{
	Physics::Physics() :
		gravity_(0, -9.81f, 0),
		fixed_timestep_(0.1f),
		current_frame_time_remaining(0.0f),
		physics_objects_()
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

	bool Physics::Add(std::shared_ptr<Lame::Physics3DComponent> i_phys_obj)
	{
		if (!i_phys_obj)
			return false;

		auto itr = std::find(physics_objects_.begin(), physics_objects_.end(), i_phys_obj);
		if (itr == physics_objects_.end())
		{
			physics_objects_.push_back(i_phys_obj);
			return true;
		}
		return false;
	}

	bool Physics::Remove(std::shared_ptr<Lame::Physics3DComponent> i_phys_obj)
	{
		if (!i_phys_obj)
			return false;

		auto itr = std::find(physics_objects_.begin(), physics_objects_.end(), i_phys_obj);
		if (itr != physics_objects_.end())
		{
			physics_objects_.erase(itr);
			return true;
		}
		return false;
	}

	bool Physics::Add(std::shared_ptr<Lame::CollisionMesh> i_col)
	{
		if (!i_col)
			return false;

		auto itr = std::find(static_meshes_.begin(), static_meshes_.end(), i_col);
		if (itr == static_meshes_.end())
		{
			static_meshes_.push_back(i_col);
			return true;
		}
		return false;
	}

	bool Physics::Remove(std::shared_ptr<Lame::CollisionMesh> i_col)
	{
		if (!i_col)
			return false;

		auto itr = std::find(static_meshes_.begin(), static_meshes_.end(), i_col);
		if (itr != static_meshes_.end())
		{
			static_meshes_.erase(itr);
			return true;
		}
		return false;
	}

	void Physics::Simulate()
	{
		for (auto itr = physics_objects_.begin(); itr != physics_objects_.end(); )
		{
			std::shared_ptr<Lame::GameObject> go = (*itr)->gameObject();
			if (go && !go->IsDestroying())
			{
				if ((*itr)->enabled())
				{
					Vector3 position, velocity;
					Predict(*itr, fixed_timestep_, position, velocity);
					go->transform().position(position);
					(*itr)->velocity(velocity);
				}

				++itr;
			}
			else
			{
				itr = physics_objects_.erase(itr);
			}
		}
		if (LameWorld::Exists())
		{
			LameWorld::Get().PhysicsUpdate(fixed_timestep_);
		}
	}

	bool Physics::RaycastAgainst(const Vector3& i_ray_start, const Vector3& i_ray_direction, std::vector<Collision::RaycastHit>& o_hit_infos) const
	{
		bool hit_something = false;
		for (auto itr = physics_objects_.begin(); itr != physics_objects_.end(); ++itr)
		{
			hit_something = (*itr)->RaycastAgainst(i_ray_start, i_ray_direction, o_hit_infos) || hit_something;
		}

		for (auto itr = static_meshes_.begin(); itr != static_meshes_.end(); ++itr)
		{
			hit_something = (*itr)->RaycastAgainst(i_ray_start, i_ray_direction, o_hit_infos) || hit_something;
		}
		return hit_something;
	}

	void Physics::Predict(std::shared_ptr<Physics3DComponent> i_comp, const float i_delta_time, Vector3& o_postion, Vector3& o_velocity) const
	{
		o_postion = i_comp->gameObject()->transform().position();
		o_velocity = i_comp->velocity();
		const Vector3 acceleration = i_comp->constant_acceleration() + gravity() * i_comp->gravity_multiplier();
		Predict(i_delta_time, o_postion, o_velocity, acceleration);
	}

	void Physics::Predict(const float i_delta_time, Vector3& io_position, Vector3& io_velocity, const Vector3 i_acceleration)
	{
		io_position += io_velocity + 0.5f * i_acceleration * i_delta_time * i_delta_time;
		io_velocity += i_acceleration * i_delta_time;
	}
}
