
#include "CollisionMesh.h"

#include "../System/Console.h"

namespace Lame
{
	CollisionMesh::CollisionMesh(std::weak_ptr<GameObject> go, const Mesh& i_mesh) :
		IComponent(go),
		mesh_(i_mesh)
	{
	}
}
