
#include "../Sprite.h"

#include "../Vertex.h"
#include "../Context.h"
#include "../Texture.h"
#include "../Effect.h"

#include "../../System/UserOutput.h"

namespace Lame
{
	Sprite* Sprite::Create(std::shared_ptr<Effect> i_effect, std::shared_ptr<Lame::Texture> i_texture, const Engine::Rectangle2D& i_screen_pos)
	{
		if (!i_effect || !i_texture)
			return nullptr;

		//Initialize effect constants
		Effect::ConstantHandle color_uniform;
		Effect::ConstantHandle texture_uniform;
		if (!i_effect->CacheConstant(Lame::Effect::Fragment, ColorUniformName, color_uniform) ||
			!i_effect->CacheConstant(Lame::Effect::Fragment, BaseTextureUniformName, texture_uniform))
		{
			System::UserOutput::Display("Unable to cache constants in Effect for Sprite");
			return nullptr;
		}

		// The usage tells Direct3D how this vertex buffer will be used
		DWORD usage = 0;
		{
			// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
			const HRESULT result = i_effect->get_context()->GetVertexProcessingUsage(usage);
			if (FAILED(result))
			{
				System::UserOutput::Display("Unable to get vertex processing usage information");
				return nullptr;
			}
			usage |= D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		}

		// Initialize the vertex format
		IDirect3DVertexDeclaration9 *vertex_declaration;
		if (!Lame::Vertex::SetVertexFormat(i_effect->get_context(), &vertex_declaration))
		{
			return nullptr;
		}

		// Create a vertex buffer
		IDirect3DVertexBuffer9 *vertex_buffer;
		{
			const UINT bufferSize = static_cast<UINT>(4 * sizeof(Vertex));
			// We will define our own vertex format
			const DWORD useSeparateVertexDeclaration = 0;
			// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
			const D3DPOOL pool = D3DPOOL_DEFAULT;
			const HRESULT result = i_effect->get_context()->get_direct3dDevice()->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, pool,
				&vertex_buffer, nullptr);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to create a vertex buffer");
				return nullptr;
			}
		}

		Sprite* sprite = new Sprite();
		if (sprite)
		{
			sprite->effect_ = i_effect;
			sprite->texture_ = i_texture;
			sprite->vertex_buffer_ = vertex_buffer;
			sprite->vertex_declaration_ = vertex_declaration;
			sprite->color_uniform_id = color_uniform;
			sprite->texture_uniform_id = texture_uniform;
		}
		else
		{
			System::UserOutput::Display("Insufficient memory to create Sprite");
		}
		return sprite;
	}

	bool Sprite::UpdateVertexData()
	{
		// Fill the vertex buffer with the triangle's vertices
		// Before the vertex buffer can be changed it must be "locked"'
		Vertex *vertexData;
		{
			const HRESULT result = vertex_buffer_->Lock(0, 0, reinterpret_cast<void**>(&vertexData), 0);
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to lock the vertex buffer");
				return false;
			}
		}
		//Fill the buffer
		{
			//TODO

			//std::memcpy(vertexData, i_vertices, i_vertex_count * sizeof(*i_vertices));

			//Preferred, but VC++ doesn't like unchecked iterators
			//std::copy(i_vertices, i_vertices + i_vertex_count, vertexData);
		}
		// The buffer must be "unlocked" before it can be used
		{
			const HRESULT result = vertex_buffer_->Unlock();
			if (FAILED(result))
			{
				System::UserOutput::Display("Direct3D failed to unlock the vertex buffer");
				return false;
			}
		}

		return true;
	}

	bool Sprite::Render()
	{
		if (!effect()->Bind() ||
			!effect()->SetConstant(Lame::Effect::Shader::Fragment, color_uniform_id, color()) ||
			!effect()->SetConstant(Lame::Effect::Shader::Fragment, texture_uniform_id, texture().get() )
			)
		{
			return false;
		}

		// Bind a specific vertex buffer to the device as a data source
		{
			HRESULT result = effect()->get_context()->get_direct3dDevice()->SetStreamSource(0, vertex_buffer_, 0, sizeof(Vertex));
			if (!SUCCEEDED(result))
				return false;
		}

		//Render the Quad
		{
			HRESULT result = effect()->get_context()->get_direct3dDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
			return SUCCEEDED(result);
		}
	}
}