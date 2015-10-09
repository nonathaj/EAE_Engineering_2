#ifndef _LAME_MESH_H
#define _LAME_MESH_H

#include <cstdint>
#include <string>

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
		static Mesh* Create(Context *&i_context, Vertex *i_vertices, size_t i_vertex_count, uint32_t *i_indices, size_t i_index_count);
		static Mesh* Create(Context *&i_context, std::string i_mesh_path);

		//Destroy a mesh
		~Mesh();

		bool Draw();

		size_t get_vertex_count() { return vertex_count_; }
		size_t get_index_count() { return index_count_; }

	private:
		Mesh(size_t i_vertex_count, size_t i_index_count, Context *&i_context);

		//Do not allow meshes to be managed without pointers
		Mesh();
		Mesh(const Mesh &i_mesh);
		Mesh& operator=(const Mesh &i_mesh);

		size_t vertex_count_;		//the number of vertices stored in this mesh
		size_t index_count_;		//the number of indices stored in this mesh
		Context *&context;

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
