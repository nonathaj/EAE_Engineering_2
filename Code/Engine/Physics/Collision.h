#ifndef _LAME_COLLISION_H
#define _LAME_COLLISION_H

#include <vector>
#include "../Core/Vector3.h"

namespace Lame
{
	class Mesh;

	namespace Collision
	{
		struct RaycastHit
		{
			Vector3 normal;
			Vector3 barycentric_coord;
			float t;

			bool IsSooner(const RaycastHit& i_other_hit) const;
		};

		int FindSoonestIndex(const std::vector<RaycastHit>& i_hit_list);
		void SortSoonestFirst(std::vector<RaycastHit>& io_hit_list);

		bool Raycast(const Vector3& i_ray_start, const Vector3& i_ray_direction, const Vector3& a, const Vector3& b, const Vector3& c, RaycastHit& o_hit_info);

		//returns the index of the closest hit, -1 if no hits were found, -2 if the mesh is not a triangle mesh
		int Raycast(const Vector3& i_ray_start, const Vector3& i_ray_direction, const Mesh& i_mesh, std::vector<RaycastHit>& o_hit_infos);
	}
}

#endif //_LAME_COLLISION_H
