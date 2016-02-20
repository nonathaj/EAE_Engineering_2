
#include "Physics3DComponent.h"

#include "../Component/GameObject.h"
#include "../Component/Transform.h"
#include "Physics.h"
#include "CollisionMesh.h"
#include "../Component/GameObject.h"

namespace Lame
{
	Physics3DComponent::Physics3DComponent(std::weak_ptr<GameObject> go) :
		IComponent(go),
		gravity_multiplier_(1.0f),
		velocity_(Vector3::zero),
		constant_acceleration_(Vector3::zero),
		collision_meshes()
	{
	}

	bool Physics3DComponent::Add(std::shared_ptr<Lame::CollisionMesh> i_col_mesh)
	{
		if (!i_col_mesh || i_col_mesh->gameObject() != gameObject())
			return false;

		auto itr = std::find(collision_meshes.begin(), collision_meshes.end(), i_col_mesh);
		if (itr == collision_meshes.end())
		{
			collision_meshes.push_back(i_col_mesh);
			return true;
		}
		return false;
	}

	bool Physics3DComponent::Remove(std::shared_ptr<Lame::CollisionMesh> i_col_mesh)
	{
		if (!i_col_mesh)
			return false;

		auto itr = std::find(collision_meshes.begin(), collision_meshes.end(), i_col_mesh);
		if (itr != collision_meshes.end())
		{
			collision_meshes.erase(itr);
			return true;
		}
		return false;
	}

	bool Physics3DComponent::RaycastAgainst(const Vector3& i_ray_start, const Vector3& i_ray_direction, std::vector<Collision::RaycastHit>& o_hit_infos) const
	{
		bool hit_something = false;
		for (auto itr = collision_meshes.begin(); itr != collision_meshes.end(); ++itr)
		{
			hit_something = (*itr)->RaycastAgainst(i_ray_start, i_ray_direction, o_hit_infos) || hit_something;
		}
		return hit_something;
	}
}
