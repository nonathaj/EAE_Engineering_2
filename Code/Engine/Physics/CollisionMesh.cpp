
#include "CollisionMesh.h"

#include "../System/Console.h"
#include "Collision.h"
#include "Physics3DComponent.h"
#include "../System/FileLoader.h"
#include "../Core/Vertex.h"
#include "../Core/Mesh.h"

namespace Lame
{
	CollisionMesh::CollisionMesh(std::weak_ptr<GameObject> go) :
		IComponent(go),
		mesh_()
	{
	}

	CollisionMesh::CollisionMesh(std::weak_ptr<GameObject> go, const Mesh& i_mesh) :
		IComponent(go),
		mesh_(i_mesh)
	{
	}

	CollisionMesh* CollisionMesh::Create(std::weak_ptr<GameObject> i_go, const std::string& i_mesh_file)
	{
		if (i_go.expired())
			return nullptr;

		uint32_t vertex_count;
		uint32_t index_count;
		Vertex *vertices;
		uint32_t *indices;
		char *fileData = File::LoadMeshData(i_mesh_file, vertex_count, index_count, vertices, indices);
		if (!fileData)
			return nullptr;

		CollisionMesh *cm = new CollisionMesh(i_go);
		if (!cm)
		{
			delete[] fileData;
			return nullptr;
		}

		cm->mesh_ = Mesh(Mesh::PrimitiveType::TriangleList, static_cast<size_t>(vertex_count), vertices, static_cast<size_t>(index_count), indices);
		delete[] fileData;
		return cm;
	}

	bool CollisionMesh::RaycastAgainst(const Vector3& i_ray_start, const Vector3& i_ray_direction, std::vector<Collision::RaycastHit>& o_hit_infos) const
	{
		return Collision::Raycast(i_ray_start, i_ray_direction, mesh_, o_hit_infos) >= 0;
	}
}
