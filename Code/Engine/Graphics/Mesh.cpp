
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <functional>
#include <string>

#include "Mesh.h"
#include "../System/UserOutput.h"
#include "../System/Console.h"
#include "../System/FileLoader.h"

#include "../Core/Vector2.h"
#include "../Core/Vector3.h"

#include "Vertex.h"

namespace Lame
{
	void Mesh::SwapIndexOrder(uint32_t *i_indices, size_t i_index_count)
	{
		for (size_t x = 0; x < i_index_count; x += 3)
			std::swap(i_indices[x], i_indices[x + 2]);
	}

	Mesh* Mesh::Create(std::shared_ptr<Context> i_context, std::string i_mesh_path)
	{
		std::string errorHeader = "Mesh loading error";

		size_t fileLength;
		char *fileData = Lame::File::LoadBinary(i_mesh_path, &fileLength);
		if (!fileData)
			return nullptr;

		//find the actual location of our data
		uint32_t *vertex_count = reinterpret_cast<uint32_t*>(fileData);
		uint32_t *index_count = vertex_count + 1;
		Vertex *vertices = reinterpret_cast<Vertex*>(index_count + 1);
		uint32_t *indices = reinterpret_cast<uint32_t*>(vertices + *vertex_count);
		
		//if the end of indices is beyond the end of the file
		if (reinterpret_cast<void*>(indices + *index_count) > fileData + fileLength)
		{
			std::stringstream error;
			error << "Loaded data for mesh " << i_mesh_path << " is invalid";
			Lame::UserOutput::Display(error.str(), errorHeader);
			delete[] fileData;
			return nullptr;
		}

		//create the mesh
		Mesh *mesh = nullptr;
#if EAE6320_PLATFORM_D3D
		mesh = CreateLeftHandedTriList(true, i_context, vertices, *vertex_count, indices, *index_count);
#elif EAE6320_PLATFORM_GL
		mesh = CreateRightHandedTriList(true, i_context, vertices, *vertex_count, indices, *index_count);
#endif

		//cleanup the loaded file
		delete[] fileData;

		return mesh;
	}

	Mesh* Mesh::CreateBox(const bool i_static, std::shared_ptr<Context> i_context, const Lame::Vector3& i_size, const Color32& i_color)
	{
		const Lame::Vector3 half = i_size / 2.0f;

		std::vector<Vertex> vertices;
		// front
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), -half.z()), Lame::Vector2(0, 1), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), -half.z()), Lame::Vector2(0, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), -half.z()), Lame::Vector2(1, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), -half.z()), Lame::Vector2(1, 1), i_color));
		// back
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), +half.z()), Lame::Vector2(1, 1), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), +half.z()), Lame::Vector2(0, 1), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), +half.z()), Lame::Vector2(0, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), +half.z()), Lame::Vector2(1, 0), i_color));
		// top
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), -half.z()), Lame::Vector2(0, 1), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), +half.z()), Lame::Vector2(0, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), +half.z()), Lame::Vector2(1, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), -half.z()), Lame::Vector2(1, 1), i_color));
		// bottom
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), -half.z()), Lame::Vector2(1, 1), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), -half.z()), Lame::Vector2(0, 1), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), +half.z()), Lame::Vector2(0, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), +half.z()), Lame::Vector2(1, 0), i_color));
		// left
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), +half.z()), Lame::Vector2(0, 1), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), +half.z()), Lame::Vector2(0, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), -half.z()), Lame::Vector2(1, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), -half.z()), Lame::Vector2(1, 1), i_color));
		// right
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), -half.z()), Lame::Vector2(0, 1), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), -half.z()), Lame::Vector2(0, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), +half.z()), Lame::Vector2(1, 0), i_color));
		vertices.push_back(Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), +half.z()), Lame::Vector2(1, 1), i_color));

		std::vector<uint32_t> indices = {
			0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};

		return CreateRightHandedTriList(i_static, i_context, vertices.data(), vertices.size(), indices.data(), indices.size());
	}

	Mesh* Mesh::CreateSphere(const bool i_static, std::shared_ptr<Context> i_context, const float i_radius, const size_t i_slice_count, const size_t i_stack_count, const Color32& i_vertex_color)
	{
		const float pi = static_cast<float>(M_PI);
		const float phi_step = pi / i_stack_count;
		const float theta_step = 2.0f * pi / i_slice_count;

		std::vector<Vertex> vertices;
		{
			vertices.reserve((i_stack_count - 1) * i_slice_count + 2);
			vertices.push_back(Lame::Vertex(Lame::Vector3(0.0f, i_radius, 0.0f), Lame::Vector2(0.0f, 0.0f), i_vertex_color));
			for (size_t x = 1; x <= i_stack_count; x++)
			{
				const float phi = x * phi_step;
				for (size_t y = 0; y <= i_slice_count; y++)
				{
					const float theta = y * theta_step;
					vertices.push_back(Lame::Vertex(
						Lame::Vector3(i_radius * std::sin(phi) * std::cos(theta), i_radius * std::cos(phi), i_radius * std::sin(phi) * std::sin(theta)),
						Lame::Vector2(theta / pi * 2, phi / pi),
						i_vertex_color));
				}
			}
			vertices.push_back(Lame::Vertex(Lame::Vector3(0.0f, -i_radius, 0.0f), Lame::Vector2(0.0f, 1.0f), i_vertex_color));
		}

		std::vector<uint32_t> indices;
		{
			for (uint32_t x = 1; x <= i_slice_count; x++)
			{
				indices.push_back(0);
				indices.push_back(x + 1);
				indices.push_back(x);
			}
			uint32_t baseIndex = 1;
			uint32_t ringVertexCount = static_cast<uint32_t>(i_slice_count + 1);
			for (uint32_t i = 0; i < i_stack_count - 1; i++)
			{
				for (uint32_t j = 0; j < i_slice_count; j++)
				{
					indices.push_back(baseIndex + i * ringVertexCount + j);
					indices.push_back(baseIndex + i * ringVertexCount + j + 1);
					indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

					indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
					indices.push_back(baseIndex + i * ringVertexCount + j + 1);
					indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
				}
			}
			uint32_t southPoleIndex = static_cast<uint32_t>(vertices.size() - 1);
			baseIndex = southPoleIndex - ringVertexCount;
			for (uint32_t i = 0; i < i_slice_count; i++)
			{
				indices.push_back(southPoleIndex);
				indices.push_back(baseIndex + i);
				indices.push_back(baseIndex + i + 1);
			}
		}
		return CreateRightHandedTriList(i_static, i_context, vertices.data(), vertices.size(), indices.data(), indices.size());
	}

	Mesh* Mesh::CreateCylinder(const bool i_static, std::shared_ptr<Context> i_context, const float i_bottom_radius, const float i_top_radius, const float i_height, const float i_slice_count, const float i_stack_count, const Color32& i_vertex_color)
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		const float pi = static_cast<float>(M_PI);

		//Cylinder
		{
			const float stackHeight = i_height / i_stack_count;
			const float radiusStep = (i_top_radius - i_bottom_radius) / i_stack_count;
			const float ringCount = i_stack_count + 1;

			for (int i = 0; i < ringCount; i++)
			{
				const float y = -0.5f * i_height + i*stackHeight;
				const float r = i_bottom_radius + i * radiusStep;
				const float dTheta = 2.0f * pi / i_slice_count;
				for (int j = 0; j <= i_slice_count; j++)
				{
					const float c = std::cos(j * dTheta);
					const float s = std::sin(j * dTheta);

					Lame::Vector3 v = Lame::Vector3(r * c, y, r * s);
					Lame::Vector2 uv = Lame::Vector2((float)j / i_slice_count, 1.0f - (float)i / i_stack_count);
					vertices.push_back(Lame::Vertex(v, uv, i_vertex_color));

				}
			}
			const uint32_t ringVertexCount = static_cast<uint32_t>(i_slice_count + 1);
			for (uint32_t i = 0; i < i_stack_count; i++)
			{
				for (uint32_t j = 0; j < i_slice_count; j++)
				{
					indices.push_back(i*ringVertexCount + j);
					indices.push_back((i + 1)*ringVertexCount + j);
					indices.push_back((i + 1)*ringVertexCount + j + 1);

					indices.push_back(i*ringVertexCount + j);
					indices.push_back((i + 1)*ringVertexCount + j + 1);
					indices.push_back(i*ringVertexCount + j + 1);
				}
			}
		}

		//top
		{
			const uint32_t baseIndex = static_cast<uint32_t>(vertices.size());

			const float y = 0.5f * i_height;
			const float dTheta = 2.0f * pi / i_slice_count;

			for (int i = 0; i <= i_slice_count; i++)
			{
				const float x = i_top_radius * std::cos(i * dTheta);
				const float z = i_top_radius * std::sin(i * dTheta);

				const float u = x / i_height + 0.5f;
				const float v = z / i_height + 0.5f;
				vertices.push_back(Lame::Vertex(Lame::Vector3(x, y, z), Lame::Vector2(u, v), i_vertex_color));
			}
			vertices.push_back(Lame::Vertex(Lame::Vector3(0, y, 0), Lame::Vector2(0.5f, 0.5f), i_vertex_color));
			const uint32_t centerIndex = static_cast<uint32_t>(vertices.size() - 1);
			for (uint32_t i = 0; i < i_slice_count; i++)
			{
				indices.push_back(centerIndex);
				indices.push_back(baseIndex + i + 1);
				indices.push_back(baseIndex + i);
			}

		}

		//bottom
		{
			const uint32_t baseIndex = static_cast<uint32_t>(vertices.size());

			const float y = -0.5f * i_height;
			const float dTheta = 2.0f * pi / i_slice_count;

			for (int i = 0; i <= i_slice_count; i++)
			{
				const float x = i_bottom_radius * std::cos(i * dTheta);
				const float z = i_bottom_radius * std::sin(i * dTheta);

				const float u = x / i_height + 0.5f;
				const float v = z / i_height + 0.5f;
				vertices.push_back(Lame::Vertex(Lame::Vector3(x, y, z), Lame::Vector2(u, v), i_vertex_color));
			}
			vertices.push_back(Lame::Vertex(Lame::Vector3(0, y, 0), Lame::Vector2(0.5f, 0.5f), i_vertex_color));
			const uint32_t centerIndex = static_cast<uint32_t>(vertices.size() - 1);
			for (int i = 0; i < i_slice_count; i++)
			{
				indices.push_back(centerIndex);
				indices.push_back(baseIndex + i);
				indices.push_back(baseIndex + i + 1);
			}
		}

		return CreateRightHandedTriList(i_static, i_context, vertices.data(), vertices.size(), indices.data(), indices.size());
	}

	Mesh* Mesh::CreateQuad(const bool i_static, std::shared_ptr<Context> i_context, const Lame::Vector2& i_extends, const Color32& i_vertex_color)
	{
		Mesh* mesh = CreateEmpty(i_static, i_context, Lame::Mesh::PrimitiveType::TriangleStrip, 4, 0);
		if (!mesh)
			return nullptr;

		Lame::Vector2 half = i_extends / 2;
		Vertex vertices[] = {
			Lame::Vertex(Lame::Vector2(-half.x(), half.y()), Lame::Vector2(0.0f, 0.0f), i_vertex_color),
			Lame::Vertex(Lame::Vector2(half.x(), half.y()), Lame::Vector2(1.0f, 0.0f), i_vertex_color),
			Lame::Vertex(Lame::Vector2(-half.x(), -half.y()), Lame::Vector2(0.0f, 1.0f), i_vertex_color),
			Lame::Vertex(Lame::Vector2(half.x(), -half.y()), Lame::Vector2(1.0f, 1.0f), i_vertex_color)
		};
		if (!mesh->UpdateVertices(vertices))
		{
			delete mesh;
			return nullptr;
		}
		else
			return mesh;
	}

	size_t Mesh::GetPrimitiveCount(const PrimitiveType i_primitive_type, const size_t i_index_count)
	{
		switch (i_primitive_type)
		{
		case Lame::Mesh::PrimitiveType::TriangleList: 
			return i_index_count / 3;
		case Lame::Mesh::PrimitiveType::TriangleStrip: 
			return i_index_count < 3 ? 0 : i_index_count - 2;
		case Lame::Mesh::PrimitiveType::LineList:
			return i_index_count / 2;
		default: 
			return 0;
		}
	}
}
