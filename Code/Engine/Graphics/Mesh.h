#ifndef _LAME_MESH_H
#define _LAME_MESH_H

#include <cstdint>
#include <string>
#include <memory>

#include "Color.h"

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Lame
{
	class Vector2;
	class Vector3;
}

namespace Lame
{
	struct Vertex;
	class Context;

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

		static Mesh* CreateEmpty(const bool i_static, std::shared_ptr<Context> i_context, PrimitiveType i_prim_type, const size_t i_vertex_count, const size_t i_index_count);

		//load a mesh with defined data
		static Mesh* CreateRightHandedTriList(const bool i_static, std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices = nullptr, size_t i_index_count = 0);
		static Mesh* CreateLeftHandedTriList(const bool i_static, std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices = nullptr, size_t i_index_count = 0);
		
		//load a mesh from the mesh binary file
		static Mesh* Create(std::shared_ptr<Context> i_context, std::string i_mesh_path);

		//create a mesh from procedural data
		static Mesh* CreateBox(const bool i_static, std::shared_ptr<Context> i_context, const Lame::Vector3& i_size, const Color32& i_vertex_color = Color32::white);
		static Mesh* CreateSphere(const bool i_static, std::shared_ptr<Context> i_context, const float i_radius, const size_t i_slice_count, const size_t i_stack_count, const Color32& i_vertex_color = Color32::white);
		static Mesh* CreateCylinder(const bool i_static, std::shared_ptr<Context> i_context, const float i_bottom_radius, const float i_top_radius, const float i_height, const float i_slice_count, const float i_stack_count, const Color32& i_vertex_color = Color32::white);
		static Mesh* CreateQuad(const bool i_static, std::shared_ptr<Context> i_context, const Lame::Vector2& i_extends, const Color32& i_vertex_color = Color32::white);

		~Mesh();

		//Render this mesh, with an optional max number of primitives (0 will render full buffer)
		bool Draw(const size_t i_max_primitives = 0) const;

		//copies the vertices/indices to the mesh data (0 amount will copy the full buffer length)
		bool UpdateVertices(const Vertex* i_vertices, const size_t i_amount = 0);
		bool UpdateIndices(const uint32_t* i_indices, const size_t i_amount = 0);

		inline PrimitiveType primitive_type() const { return primitive_type_; }
		inline void primitive_type(const PrimitiveType i_prim) { primitive_type_ = i_prim; }
		
		inline size_t get_vertex_count() const { return vertex_count_; }
		inline size_t get_index_count() const { return index_count_; }
		inline std::shared_ptr<Context> get_context() const { return context; }
	private:
		Mesh(size_t i_vertex_count, size_t i_index_count, PrimitiveType i_prim_type, std::shared_ptr<Context> i_context);

		//Do not allow meshes to be managed without pointers
		Mesh();
		Mesh(const Mesh &i_mesh);
		Mesh& operator=(const Mesh &i_mesh);

		std::shared_ptr<Context> context;

		//Swaps the order of indices (between right and left handed-ness) without error checking
		static void SwapIndexOrder(uint32_t *i_indices, size_t i_index_count);

#if EAE6320_PLATFORM_D3D
		IDirect3DVertexBuffer9 *vertex_buffer_;
		IDirect3DIndexBuffer9 *index_buffer_;
		IDirect3DVertexDeclaration9 *vertex_declaration_;
#elif EAE6320_PLATFORM_GL
		GLuint vertex_array_id_;
#endif

		PrimitiveType primitive_type_;
		size_t vertex_count_;		//the number of vertices stored in this mesh
		size_t index_count_;		//the number of indices stored in this mesh
	};
}

#endif //_LAME_MESH_H
