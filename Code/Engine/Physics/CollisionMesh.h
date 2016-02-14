#ifndef _LAME_PHYSICS_H
#define _LAME_PHYSICS_H

#include "../Core/Vector3.h"
#include "../Core/Mesh.h"

namespace Lame
{
	class CollisionMesh
	{
	public:
		Mesh& mesh() { return mesh_; }
		void mesh(const Mesh& i_mesh) { mesh_ = i_mesh; }
	private:
		Mesh mesh_;
	};
}

#endif //_LAME_PHYSICS_H