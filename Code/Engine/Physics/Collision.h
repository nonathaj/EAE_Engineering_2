#ifndef _LAME_COLLISION_H
#define _LAME_COLLISION_H

namespace Lame
{
	class Vector3;

	namespace Collision
	{
		bool IntersectSegmentTriangle(Vector3 i_ray_start, Vector3 i_ray_direction, Vector3 a, Vector3 b, Vector3 c, Vector3& o_normal, Vector3& o_barycentric_coord, float& o_t);
	}
}

#endif //_LAME_COLLISION_H