
#include "Collision.h"
#include "../Core/Vector3.h"
#include "../Core/Mesh.h"
#include "../System/Console.h"

namespace Lame
{
	namespace Collision
	{
		bool RaycastHit::IsSooner(const RaycastHit& i_other_hit) const
		{
			return t < i_other_hit.t;
		}

		bool Raycast(const Vector3& i_ray_start, const Vector3& i_ray_direction, const Vector3& a, const Vector3& b, const Vector3& c, RaycastHit& o_hit_info)
		{
			Vector3 ab = b - a;
			Vector3 ac = c - a;

			// Compute triangle normal. Can be precalculated or cached if
			// intersecting multiple segments against the same triangle
			o_hit_info.normal = ab.cross(ac);

			// Compute denominator d. If d <= 0, segment is parallel to or points
			// away from triangle, so exit early
			float d = i_ray_direction.dot(o_hit_info.normal);
			if (d <= 0.0f) 
				return false;

			// Compute intersection t value of pq with plane of triangle. A ray
			// intersects if 0 <= t. Segment intersects if 0 <= t <= 1. Delay
			// dividing by d until intersection has been found to pierce triangle
			Vector3 ap = i_ray_start - a;
			o_hit_info.t = ap.dot(o_hit_info.normal);
			if (o_hit_info.t < 0.0f)
				return false;
			else if (o_hit_info.t > d)
				return false; // For segment; exclude this code line for a ray test

			// Compute barycentric coordinate components and test if within bounds
			Vector3 e = i_ray_direction.cross(ap);
			o_hit_info.barycentric_coord.y(ac.dot(e));
			if (o_hit_info.barycentric_coord.y() < 0.0f || o_hit_info.barycentric_coord.y() > d)
				return false;
			o_hit_info.barycentric_coord.z(-ab.dot(e));
			if (o_hit_info.barycentric_coord.z() < 0.0f || o_hit_info.barycentric_coord.y() + o_hit_info.barycentric_coord.z() > d)
				return false;

			// Segment/ray intersects triangle. Perform delayed division and
			// compute the last barycentric coordinate component
			float ood = 1.0f / d;
			o_hit_info.t *= ood;
			o_hit_info.barycentric_coord.y(o_hit_info.barycentric_coord.y() * ood);
			o_hit_info.barycentric_coord.z(o_hit_info.barycentric_coord.z() * ood);
			o_hit_info.barycentric_coord.x(1.0f - o_hit_info.barycentric_coord.y() - o_hit_info.barycentric_coord.z());
			return true;
		}

		int Raycast(const Vector3& i_ray_start, const Vector3& i_ray_direction, const Mesh& i_mesh, std::vector<RaycastHit>& o_hit_info)
		{
			if (!Mesh::IsTriangles(i_mesh.primitive_type()))
				return -2;

			const size_t start_index = o_hit_info.size();	//we can already have a list of hit items
			int soonest_index = -1;
			const size_t primitive_count = i_mesh.primitive_count();
			for (size_t x = 0; x < primitive_count; x++)
			{
				std::vector<Vertex> primitive_vertices;
				RaycastHit hitinfo;
				if (i_mesh.GetPrimitive(primitive_vertices, x) &&
					primitive_vertices.size() == 3 &&
					Raycast(i_ray_start, i_ray_direction, 
						primitive_vertices[0].position, primitive_vertices[1].position, 
						primitive_vertices[2].position, hitinfo) )
				{
					o_hit_info.push_back(hitinfo);
					if (soonest_index == -1 || hitinfo.IsSooner(o_hit_info[soonest_index]))
					{
						soonest_index = static_cast<int>(start_index + x);
					}
				}
			}
			return o_hit_info.size() > 0 ? soonest_index : -1;
		}

		void SortSoonestFirst(std::vector<RaycastHit>& io_hit_list)
		{
			std::sort(
				io_hit_list.begin(), 
				io_hit_list.end(),
				[](const RaycastHit& a, const RaycastHit& b) { return a.IsSooner(b); }
			);
		}

		int FindSoonestIndex(const std::vector<RaycastHit>& i_hit_list)
		{
			if (i_hit_list.size() == 0)
				return -1;

			int index = 0;
			for (int i = 1; i < i_hit_list.size(); i++)
			{
				if (i_hit_list[i].IsSooner(i_hit_list[index]))
					index = i;
			}
			return index;
		}
	}
}
