#ifndef _LAME_PHYSICS_H
#define _LAME_PHYSICS_H

#include "../Core/Vector3.h"
#include "../Core/Mesh.h"
#include "../Component/IComponent.h"

namespace Lame
{
	class CollisionMesh : public IComponent
	{
	public:
		CollisionMesh(std::weak_ptr<GameObject> go, const Mesh& i_mesh);

		Mesh& mesh() { return mesh_; }
		void mesh(const Mesh& i_mesh) { mesh_ = i_mesh; }
	private:
		Mesh mesh_;
	};
}

#endif //_LAME_PHYSICS_H
