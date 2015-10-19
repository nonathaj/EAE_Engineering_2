#ifndef _LAME_MESH_H
#define _LAME_MESH_H

#include <cstdint>
#include <string>
#include <memory>

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Lame
{
	struct Vertex;
	class Context;

	class Mesh
	{
	public:
		//load a mesh with defined data
		static Mesh* CreateRightHanded(std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count);
		static Mesh* CreateLeftHanded(std::shared_ptr<Context> i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count);
		
		//load a mesh from the mesh binary file
		static Mesh* Create(std::shared_ptr<Context> i_context, std::string i_mesh_path);

		//Destroy a mesh
		~Mesh();

		//render the mesh
		bool Draw();

		inline size_t get_vertex_count() { return vertex_count_; }
		inline size_t get_index_count() { return index_count_; }
		inline std::shared_ptr<Context> get_context() { return context; }
	private:
		Mesh(size_t i_vertex_count, size_t i_index_count, std::shared_ptr<Context> i_context);

		//Do not allow meshes to be managed without pointers
		Mesh();
		Mesh(const Mesh &i_mesh);
		Mesh& operator=(const Mesh &i_mesh);

		size_t vertex_count_;		//the number of vertices stored in this mesh
		size_t index_count_;		//the number of indices stored in this mesh
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
	};
}

#endif //_LAME_MESH_H
