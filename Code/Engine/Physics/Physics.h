#ifndef _LAME_PHYSICS_H
#define _LAME_PHYSICS_H

#include <memory>

#include "../Core/Singleton.h"
#include "../Core/Vector3.h"
#include "Collision.h"

namespace Lame
{
	class CollisionMesh;
	class Physics3DComponent;

	class Physics
	{
	public:
		bool Setup();

		void Tick(float deltaTime);

		bool Add(std::shared_ptr<Lame::Physics3DComponent> i_phys_obj);
		bool Remove(std::shared_ptr<Lame::Physics3DComponent> i_phys_obj);

		bool Add(std::shared_ptr<Lame::CollisionMesh> i_col);
		bool Remove(std::shared_ptr<Lame::CollisionMesh> i_col);

		Vector3 gravity() const { return gravity_; }
		void gravity(const Vector3& i_gravity) { gravity_ = i_gravity; }

		float fixed_timestep() const { return fixed_timestep_; }
		void fixed_timestep(const float i_fixed_timestep) { fixed_timestep_ = i_fixed_timestep; }

		void Predict(std::shared_ptr<Physics3DComponent> i_comp, const float i_delta_time, Vector3& o_postion, Vector3& o_velocity) const;

		static void Predict(const float i_delta_time, Vector3& io_postion, Vector3& io_velocity, const Vector3 i_acceleration);

		bool RaycastAgainst(const Vector3& i_ray_start, const Vector3& i_ray_direction, std::vector<Collision::RaycastHit>& o_hit_infos) const;
	private:
		Physics();

		void Simulate();

		std::vector<std::shared_ptr<Lame::Physics3DComponent>> physics_objects_;
		std::vector<std::shared_ptr<Lame::CollisionMesh>> static_meshes_;

		float current_frame_time_remaining;

		float fixed_timestep_;
		Vector3 gravity_;

		friend Lame::Singleton<Lame::Physics>;
	};
}

typedef Lame::Singleton<Lame::Physics> LamePhysics;

#endif //_LAME_PHYSICS_H
