#ifndef _LAME_MESH_H
#define _LAME_MESH_H

#include <cstdint>
#include <vector>
#include "Vertex.h"

namespace Lame
{
	class Mesh
	{
	public:
		enum class PrimitiveType
		{
			TriangleList,	//Sequence of Independent Triangles
			TriangleStrip,	//Sequence of Sliding Triangles
			TriangleFan,	//Sequence of Sliding Triangles based on the first vertex rendered
			LineList,		//Sequence of Independent Lines
			LineStrip,		//Sequence of Sliding Lines
		};
		static size_t GetPrimitiveCount(const PrimitiveType i_primitive_type, const size_t i_index_count);
		static bool IsTriangles(const PrimitiveType i_primitive_type);

		Mesh() {}
		Mesh(const PrimitiveType i_prim_type) : primitive_type_(i_prim_type) { }

		std::vector<Vertex>& vertices() { return vertices_; }
		std::vector<uint32_t>& indices() { return indices_; }

		const std::vector<Vertex>& vertices_RO() const { return vertices_; }
		const std::vector<uint32_t>& indices_RO() const { return indices_; }
		
		PrimitiveType primitive_type() const { return primitive_type_; }
		void primitive_type(const PrimitiveType i_prim_type) { primitive_type_ = i_prim_type; }

		size_t primitive_count() const { return GetPrimitiveCount(primitive_type_, has_indices() ? indices_.size() : vertices_.size()); }
		inline bool has_indices() const { return indices_.size() > 0; }

		void SetBox(const Lame::Vector3& i_size, const Color32& i_vertex_color = Color32::white);
		void SetSphere(const float i_radius, const size_t i_slice_count, const size_t i_stack_count, const Color32& i_vertex_color = Color32::white);
		void SetCylinder(const float i_bottom_radius, const float i_top_radius, const float i_height, const float i_slice_count, const float i_stack_count, const Color32& i_vertex_color = Color32::white);
		void SetQuad(const Lame::Vector2& i_extends, const Color32& i_vertex_color = Color32::white);

		bool GetPrimitive(std::vector<Vertex>& o_primitive_vertices, const size_t i_primitive_index) const;

		bool SwapTriangleListNormals();	//swaps the normals for all triangles, assuming this mesh stores TriangleList primitive

		template<typename T>
		static bool SwapTriangleListNormals(std::vector<T>& io_tri_list);
	private:
		std::vector<Vertex> vertices_;
		std::vector<uint32_t> indices_;
		PrimitiveType primitive_type_;
	};

	template<typename T>
	bool Mesh::SwapTriangleListNormals(std::vector<T>& io_tri_list)
	{
		if (io_tri_list.size() % 3 != 0)
			return false;
		for (size_t x = 0; x < io_tri_list.size(); x += 3)
			std::swap(io_tri_list[x], io_tri_list[x + 2]);
		return true;
	}
}

#endif //_LAME_MESH_H
