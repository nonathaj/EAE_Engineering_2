
#include "CollisionMesh.h"

#include "../System/Console.h"
#include "Collision.h"

namespace Lame
{
	CollisionMesh::CollisionMesh(std::weak_ptr<GameObject> go, const Mesh& i_mesh) :
		IComponent(go),
		mesh_(i_mesh)
	{
	}

	bool CollisionMesh::RaycastAgainst(const Vector3& i_ray_start, const Vector3& i_ray_direction, std::vector<Collision::RaycastHit>& o_hit_infos) const
	{
		return Collision::Raycast(i_ray_start, i_ray_direction, mesh_, o_hit_infos);
	}
}
