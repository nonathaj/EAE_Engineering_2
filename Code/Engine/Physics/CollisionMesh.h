#ifndef _LAME_COLLISIONMESH_H
#define _LAME_COLLISIONMESH_H

#include <memory>

#include "../Core/Vector3.h"
#include "../Core/Mesh.h"
#include "../Component/IComponent.h"

namespace Lame
{
	namespace Collision
	{
		struct RaycastHit;
	}

	class Physics3DComponent;

	class CollisionMesh : public IComponent
	{
	public:
		CollisionMesh(std::weak_ptr<GameObject> go);
		CollisionMesh(std::weak_ptr<GameObject> go, const Mesh& i_mesh);
		
		static CollisionMesh* Create(std::weak_ptr<GameObject> i_go, const std::string& i_mesh_file);

		Mesh& mesh() { return mesh_; }
		void mesh(const Mesh& i_mesh) { mesh_ = i_mesh; }

		bool RaycastAgainst(const Vector3& i_ray_start, const Vector3& i_ray_direction, std::vector<Collision::RaycastHit>& o_hit_infos) const;
	private:
		Mesh mesh_;

		std::weak_ptr<Physics3DComponent> physics_component;
	};
}

#endif //_LAME_COLLISIONMESH_H
