
#include "Collision.h"
#include "../Core/Vector3.h"

namespace Lame
{
	namespace Collision
	{
		bool IntersectSegmentTriangle(Vector3 i_ray_start, Vector3 i_ray_direction, Vector3 a, Vector3 b, Vector3 c,
			Vector3& o_normal, Vector3& o_barycentric_coord, float& o_t)
		{
			Vector3 ab = b - a;
			Vector3 ac = c - a;

			// Compute triangle normal. Can be precalculated or cached if
			// intersecting multiple segments against the same triangle
			o_normal = ab.cross(ac);

			// Compute denominator d. If d <= 0, segment is parallel to or points
			// away from triangle, so exit early
			float d = i_ray_direction.dot(o_normal);
			if (d <= 0.0f) 
				return false;

			// Compute intersection t value of pq with plane of triangle. A ray
			// intersects if 0 <= t. Segment intersects if 0 <= t <= 1. Delay
			// dividing by d until intersection has been found to pierce triangle
			Vector3 ap = i_ray_start - a;
			o_t = ap.dot(o_normal);
			if (o_t < 0.0f)
				return false;
			else if (o_t > d)
				return false; // For segment; exclude this code line for a ray test

			// Compute barycentric coordinate components and test if within bounds
			Vector3 e = i_ray_direction.cross(ap);
			o_barycentric_coord.y(ac.dot(e));
			if (o_barycentric_coord.y() < 0.0f || o_barycentric_coord.y() > d) 
				return false;
			o_barycentric_coord.z(-ab.dot(e));
			if (o_barycentric_coord.z() < 0.0f || o_barycentric_coord.y() + o_barycentric_coord.z() > d) 
				return false;

			// Segment/ray intersects triangle. Perform delayed division and
			// compute the last barycentric coordinate component
			float ood = 1.0f / d;
			o_t *= ood;
			o_barycentric_coord.y(o_barycentric_coord.y() * ood);
			o_barycentric_coord.z(o_barycentric_coord.z() * ood);
			o_barycentric_coord.x(1.0f - o_barycentric_coord.y() - o_barycentric_coord.z());
			return true;
		}
	}
}