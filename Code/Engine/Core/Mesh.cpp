
#include "Mesh.h"

namespace Lame
{
	size_t Mesh::GetPrimitiveCount(const Lame::Mesh::PrimitiveType i_primitive_type, const size_t i_index_count)
	{
		switch (i_primitive_type)
		{
		case Lame::Mesh::PrimitiveType::TriangleList:
			return i_index_count / 3;
		case Lame::Mesh::PrimitiveType::TriangleStrip:
			return i_index_count < 3 ? 0 : i_index_count - 2;
		case Lame::Mesh::PrimitiveType::TriangleFan:
			return i_index_count < 3 ? 0 : i_index_count - 2;
		case Lame::Mesh::PrimitiveType::LineList:
			return i_index_count / 2;
		case Lame::Mesh::PrimitiveType::LineStrip:
			return i_index_count < 2 ? 0 : i_index_count - 1;
		default:
			return 0;
		}
	}

	bool Lame::Mesh::IsTriangles(const Lame::Mesh::PrimitiveType i_primitive_type)
	{
		switch (i_primitive_type)
		{
		case Lame::Mesh::PrimitiveType::TriangleList:
		case Lame::Mesh::PrimitiveType::TriangleStrip:
		case Lame::Mesh::PrimitiveType::TriangleFan:
			return true;
		case Lame::Mesh::PrimitiveType::LineList:
		case Lame::Mesh::PrimitiveType::LineStrip:
		default:
			return false;
		}
	}

	bool Mesh::SwapTriangleListNormals()
	{
		if (!IsTriangles(primitive_type_))
			return false;

		if (has_indices())
			return SwapTriangleListNormals(indices_);
		else
			return SwapTriangleListNormals(vertices_);
	}

	bool Mesh::GetPrimitive(std::vector<Vertex>& o_primitive_vertices, const size_t i_primitive_index) const
	{
		if (i_primitive_index >= primitive_count())
			return false;

		//find the indices of the primitive
		std::vector<size_t> primitive_indices;
		switch (primitive_type_)
		{
		case Lame::Mesh::PrimitiveType::TriangleList:
			{
				size_t prim_start = i_primitive_index * 3;
				if (has_indices())
				{
					primitive_indices.push_back(static_cast<size_t>(indices_[prim_start]));
					primitive_indices.push_back(static_cast<size_t>(indices_[prim_start + 1]));
					primitive_indices.push_back(static_cast<size_t>(indices_[prim_start + 2]));
				}
				else
				{
					primitive_indices.push_back(prim_start);
					primitive_indices.push_back(prim_start + 1);
					primitive_indices.push_back(prim_start + 2);
				}
			}

			break;
		case Lame::Mesh::PrimitiveType::TriangleStrip:

			if (has_indices())
			{
				primitive_indices.push_back(static_cast<size_t>(indices_[i_primitive_index]));
				primitive_indices.push_back(static_cast<size_t>(indices_[i_primitive_index + 1]));
				primitive_indices.push_back(static_cast<size_t>(indices_[i_primitive_index + 2]));
			}
			else
			{
				primitive_indices.push_back(i_primitive_index);
				primitive_indices.push_back(i_primitive_index + 1);
				primitive_indices.push_back(i_primitive_index + 2);
			}

			break;
		case Lame::Mesh::PrimitiveType::TriangleFan:

			if (has_indices())
			{
				primitive_indices.push_back(static_cast<size_t>(indices_[0]));
				primitive_indices.push_back(static_cast<size_t>(indices_[i_primitive_index + 1]));
				primitive_indices.push_back(static_cast<size_t>(indices_[i_primitive_index + 2]));
			}
			else
			{
				primitive_indices.push_back(0);
				primitive_indices.push_back(i_primitive_index + 1);
				primitive_indices.push_back(i_primitive_index + 2);
			}

			break;
		case Lame::Mesh::PrimitiveType::LineList:
			{
				size_t prim_start = i_primitive_index * 2;
				if (has_indices())
				{
					primitive_indices.push_back(static_cast<size_t>(indices_[prim_start]));
					primitive_indices.push_back(static_cast<size_t>(indices_[prim_start + 1]));
				}
				else
				{
					primitive_indices.push_back(prim_start);
					primitive_indices.push_back(prim_start + 1);
				}
			}
			break;
		case Lame::Mesh::PrimitiveType::LineStrip:

			if (has_indices())
			{
				primitive_indices.push_back(static_cast<size_t>(indices_[i_primitive_index]));
				primitive_indices.push_back(static_cast<size_t>(indices_[i_primitive_index + 1]));
			}
			else
			{
				primitive_indices.push_back(i_primitive_index);
				primitive_indices.push_back(i_primitive_index + 1);
			}

			break;
		default:

			return false;
		}

		o_primitive_vertices.clear();
		for (size_t x = 0; x < primitive_indices.size(); x++)
		{
			o_primitive_vertices.push_back(vertices_[primitive_indices[x]]);
		}
		return true;
	}

	void Mesh::SetBox(const Lame::Vector3& i_size, const Color32& i_color)
	{
		const Lame::Vector3 half = i_size / 2.0f;

		vertices_ = {
			// front
			Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), -half.z()), Lame::Vector2(0, 1), i_color),
			Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), -half.z()), Lame::Vector2(0, 0), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), -half.z()), Lame::Vector2(1, 0), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), -half.z()), Lame::Vector2(1, 1), i_color),
			// back
			Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), +half.z()), Lame::Vector2(1, 1), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), +half.z()), Lame::Vector2(0, 1), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), +half.z()), Lame::Vector2(0, 0), i_color),
			Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), +half.z()), Lame::Vector2(1, 0), i_color),
			// top
			Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), -half.z()), Lame::Vector2(0, 1), i_color),
			Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), +half.z()), Lame::Vector2(0, 0), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), +half.z()), Lame::Vector2(1, 0), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), -half.z()), Lame::Vector2(1, 1), i_color),
			// bottom
			Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), -half.z()), Lame::Vector2(1, 1), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), -half.z()), Lame::Vector2(0, 1), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), +half.z()), Lame::Vector2(0, 0), i_color),
			Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), +half.z()), Lame::Vector2(1, 0), i_color),
			// left
			Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), +half.z()), Lame::Vector2(0, 1), i_color),
			Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), +half.z()), Lame::Vector2(0, 0), i_color),
			Lame::Vertex(Lame::Vector3(-half.x(), +half.y(), -half.z()), Lame::Vector2(1, 0), i_color),
			Lame::Vertex(Lame::Vector3(-half.x(), -half.y(), -half.z()), Lame::Vector2(1, 1), i_color),
			// right
			Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), -half.z()), Lame::Vector2(0, 1), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), -half.z()), Lame::Vector2(0, 0), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), +half.y(), +half.z()), Lame::Vector2(1, 0), i_color),
			Lame::Vertex(Lame::Vector3(+half.x(), -half.y(), +half.z()), Lame::Vector2(1, 1), i_color)
		};

		indices_ = {
			0,1,2,0,2,3,
			4,5,6,4,6,7,
			8,9,10,8,10,11,
			12,13,14,12,14,15,
			16,17,18,16,18,19,
			20,21,22,20,22,23
		};
	}

	void Mesh::SetSphere(const float i_radius, const size_t i_slice_count, const size_t i_stack_count, const Color32& i_vertex_color)
	{
		vertices_.clear();
		indices_.clear();
		const float pi = static_cast<float>(M_PI);
		const float phi_step = pi / i_stack_count;
		const float theta_step = 2.0f * pi / i_slice_count;

		//vertices
		{
			vertices_.reserve((i_stack_count - 1) * i_slice_count + 2);
			vertices_.push_back(Lame::Vertex(Lame::Vector3(0.0f, i_radius, 0.0f), Lame::Vector2(0.0f, 0.0f), i_vertex_color));
			for (size_t x = 1; x <= i_stack_count; x++)
			{
				const float phi = x * phi_step;
				for (size_t y = 0; y <= i_slice_count; y++)
				{
					const float theta = y * theta_step;
					vertices_.push_back(Lame::Vertex(
						Lame::Vector3(i_radius * std::sin(phi) * std::cos(theta), i_radius * std::cos(phi), i_radius * std::sin(phi) * std::sin(theta)),
						Lame::Vector2(theta / pi * 2, phi / pi),
						i_vertex_color));
				}
			}
			vertices_.push_back(Lame::Vertex(Lame::Vector3(0.0f, -i_radius, 0.0f), Lame::Vector2(0.0f, 1.0f), i_vertex_color));
		}

		//indices
		{
			for (uint32_t x = 1; x <= i_slice_count; x++)
			{
				indices_.push_back(0);
				indices_.push_back(x + 1);
				indices_.push_back(x);
			}
			uint32_t baseIndex = 1;
			uint32_t ringVertexCount = static_cast<uint32_t>(i_slice_count + 1);
			for (uint32_t i = 0; i < i_stack_count - 1; i++)
			{
				for (uint32_t j = 0; j < i_slice_count; j++)
				{
					indices_.push_back(baseIndex + i * ringVertexCount + j);
					indices_.push_back(baseIndex + i * ringVertexCount + j + 1);
					indices_.push_back(baseIndex + (i + 1) * ringVertexCount + j);

					indices_.push_back(baseIndex + (i + 1) * ringVertexCount + j);
					indices_.push_back(baseIndex + i * ringVertexCount + j + 1);
					indices_.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
				}
			}
			uint32_t southPoleIndex = static_cast<uint32_t>(vertices_.size() - 1);
			baseIndex = southPoleIndex - ringVertexCount;
			for (uint32_t i = 0; i < i_slice_count; i++)
			{
				indices_.push_back(southPoleIndex);
				indices_.push_back(baseIndex + i);
				indices_.push_back(baseIndex + i + 1);
			}
		}
	}

	void Mesh::SetCylinder(const float i_bottom_radius, const float i_top_radius, const float i_height, const float i_slice_count, const float i_stack_count, const Color32& i_vertex_color)
	{
		vertices_.clear();
		indices_.clear();
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
					vertices_.push_back(Lame::Vertex(v, uv, i_vertex_color));

				}
			}
			const uint32_t ringVertexCount = static_cast<uint32_t>(i_slice_count + 1);
			for (uint32_t i = 0; i < i_stack_count; i++)
			{
				for (uint32_t j = 0; j < i_slice_count; j++)
				{
					indices_.push_back(i*ringVertexCount + j);
					indices_.push_back((i + 1)*ringVertexCount + j);
					indices_.push_back((i + 1)*ringVertexCount + j + 1);

					indices_.push_back(i*ringVertexCount + j);
					indices_.push_back((i + 1)*ringVertexCount + j + 1);
					indices_.push_back(i*ringVertexCount + j + 1);
				}
			}
		}

		//top
		{
			const uint32_t baseIndex = static_cast<uint32_t>(vertices_.size());

			const float y = 0.5f * i_height;
			const float dTheta = 2.0f * pi / i_slice_count;

			for (int i = 0; i <= i_slice_count; i++)
			{
				const float x = i_top_radius * std::cos(i * dTheta);
				const float z = i_top_radius * std::sin(i * dTheta);

				const float u = x / i_height + 0.5f;
				const float v = z / i_height + 0.5f;
				vertices_.push_back(Lame::Vertex(Lame::Vector3(x, y, z), Lame::Vector2(u, v), i_vertex_color));
			}
			vertices_.push_back(Lame::Vertex(Lame::Vector3(0, y, 0), Lame::Vector2(0.5f, 0.5f), i_vertex_color));
			const uint32_t centerIndex = static_cast<uint32_t>(vertices_.size() - 1);
			for (uint32_t i = 0; i < i_slice_count; i++)
			{
				indices_.push_back(centerIndex);
				indices_.push_back(baseIndex + i + 1);
				indices_.push_back(baseIndex + i);
			}

		}

		//bottom
		{
			const uint32_t baseIndex = static_cast<uint32_t>(vertices_.size());

			const float y = -0.5f * i_height;
			const float dTheta = 2.0f * pi / i_slice_count;

			for (int i = 0; i <= i_slice_count; i++)
			{
				const float x = i_bottom_radius * std::cos(i * dTheta);
				const float z = i_bottom_radius * std::sin(i * dTheta);

				const float u = x / i_height + 0.5f;
				const float v = z / i_height + 0.5f;
				vertices_.push_back(Lame::Vertex(Lame::Vector3(x, y, z), Lame::Vector2(u, v), i_vertex_color));
			}
			vertices_.push_back(Lame::Vertex(Lame::Vector3(0, y, 0), Lame::Vector2(0.5f, 0.5f), i_vertex_color));
			const uint32_t centerIndex = static_cast<uint32_t>(vertices_.size() - 1);
			for (int i = 0; i < i_slice_count; i++)
			{
				indices_.push_back(centerIndex);
				indices_.push_back(baseIndex + i);
				indices_.push_back(baseIndex + i + 1);
			}
		}
	}

	void Mesh::SetQuad(const Lame::Vector2& i_extends, const Color32& i_vertex_color)
	{
		Lame::Vector2 half = i_extends / 2;
		vertices_ = {
			Lame::Vertex(Lame::Vector2(-half.x(), half.y()), Lame::Vector2(0.0f, 0.0f), i_vertex_color),
			Lame::Vertex(Lame::Vector2(half.x(), half.y()), Lame::Vector2(1.0f, 0.0f), i_vertex_color),
			Lame::Vertex(Lame::Vector2(-half.x(), -half.y()), Lame::Vector2(0.0f, 1.0f), i_vertex_color),
			Lame::Vertex(Lame::Vector2(half.x(), -half.y()), Lame::Vector2(1.0f, 1.0f), i_vertex_color)
		};
		indices_.clear();
	}
}
