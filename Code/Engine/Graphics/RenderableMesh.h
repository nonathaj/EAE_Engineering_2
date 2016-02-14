#ifndef _LAME_RENDERABLEMESH_H
#define _LAME_RENDERABLEMESH_H

#include <cstdint>
#include <string>
#include <memory>

#include "../Core/Color.h"
#include "../Core/Mesh.h"

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

	class RenderableMesh
	{
	public:
		static RenderableMesh* CreateEmpty(const bool i_static, std::shared_ptr<Context> i_context, Mesh::PrimitiveType i_prim_type, const size_t i_vertex_count, const size_t i_index_count);

		//load a mesh with defined data
		static RenderableMesh* CreateRightHandedTriList(const bool i_static, std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices = nullptr, size_t i_index_count = 0);
		static RenderableMesh* CreateLeftHandedTriList(const bool i_static, std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices = nullptr, size_t i_index_count = 0);
		
		//load a mesh from the mesh binary file
		static RenderableMesh* Create(const bool i_static, std::shared_ptr<Context> i_context, const std::string& i_mesh_path);

		static RenderableMesh* Create(const bool i_static, std::shared_ptr<Context> i_context, const Mesh& i_mesh);

		~RenderableMesh();

		//Render this mesh, with an optional max number of primitives (0 will render full buffer)
		bool Draw(const size_t i_max_primitives = 0) const;

		//copies the vertices/indices to the mesh data (0 amount will copy the full buffer length)
		bool UpdateVertices(const Vertex* i_vertices, const size_t i_amount = 0);
		bool UpdateIndices(const uint32_t* i_indices, const size_t i_amount = 0);

		inline Mesh::PrimitiveType primitive_type() const { return primitive_type_; }
		inline void primitive_type(const Mesh::PrimitiveType i_prim) { primitive_type_ = i_prim; }
		size_t primitive_count() const;
		
		inline size_t get_vertex_count() const { return vertex_count_; }
		inline size_t get_index_count() const { return index_count_; }
		inline std::shared_ptr<Context> get_context() const { return context; }
	private:
		RenderableMesh(size_t i_vertex_count, size_t i_index_count, Mesh::PrimitiveType i_prim_type, std::shared_ptr<Context> i_context);

		//Do not allow meshes to be managed without pointers
		RenderableMesh();
		RenderableMesh(const RenderableMesh &i_mesh);
		RenderableMesh& operator=(const RenderableMesh &i_mesh);

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

		Mesh::PrimitiveType primitive_type_;
		size_t vertex_count_;		//the number of vertices stored in this mesh
		size_t index_count_;		//the number of indices stored in this mesh
	};
}

#endif //_LAME_RENDERABLEMESH_H
