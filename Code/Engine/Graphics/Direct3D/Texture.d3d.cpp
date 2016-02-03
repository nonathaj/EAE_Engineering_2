
#include "../Texture.h"

#include <sstream>
#include <D3dx9tex.h>

#include "../Context.h"
#include "../../Core/Vector2.h"
#include "../../System/UserOutput.h"

namespace Lame
{
	Texture* Texture::Create(std::shared_ptr<Context> i_context, const std::string& i_path)
	{
		const unsigned int useDimensionsFromFile = D3DX_DEFAULT_NONPOW2;
		const unsigned int useMipMapsFromFile = D3DX_FROM_FILE;
		const DWORD staticTexture = 0;
		const D3DFORMAT useFormatFromFile = D3DFMT_FROM_FILE;
		const D3DPOOL letD3dManageMemory = D3DPOOL_MANAGED;
		const DWORD useDefaultFiltering = D3DX_DEFAULT;
		const D3DCOLOR noColorKey = 0;
		D3DXIMAGE_INFO sourceInfo;
		PALETTEENTRY* noColorPalette = nullptr;
		IDirect3DTexture9 *d3dtexture = nullptr;
		const HRESULT result = D3DXCreateTextureFromFileEx(i_context->get_direct3dDevice(), i_path.c_str(), useDimensionsFromFile, useDimensionsFromFile, useMipMapsFromFile,
			staticTexture, useFormatFromFile, letD3dManageMemory, useDefaultFiltering, useDefaultFiltering, noColorKey, &sourceInfo, noColorPalette,
			&d3dtexture);
		if (!SUCCEEDED(result))
		{
			std::stringstream error;
			error << "DirectX failed to load a texture from " << i_path << ": ";
			switch (result)
			{
			case D3DERR_INVALIDCALL:
				error << "Invalid function call";
				break;
			case D3DERR_NOTAVAILABLE:
				error << "D3DXCreateTextureFromFileEx function not available";
				break;
			case D3DERR_OUTOFVIDEOMEMORY:
				error << "Out of video memory";
				break;
			case D3DXERR_INVALIDDATA:
				error << "Invalid data passed to D3DXCreateTextureFromFileEx";
				break;
			case E_OUTOFMEMORY:
				error << "Out of memory";
				break;
			}
			System::UserOutput::Display(error.str(), "DirectX Texture Load Error");
			return nullptr;
		}

		Texture *texture = new Texture();
		if (texture)
		{
			texture->texture_ = d3dtexture;
			texture->width_ = static_cast<size_t>(sourceInfo.Width);
			texture->height_ = static_cast<size_t>(sourceInfo.Height);
			return texture;
		}
		else
		{
			std::stringstream error;
			error << "Insufficient memory to create texture for " << i_path;
			System::UserOutput::Display(error.str(), "Texture Load Error");
			d3dtexture->Release();
			return nullptr;
		}
	}

	Texture::~Texture()
	{
		if (texture_ != nullptr)
		{
			texture_->Release();
			texture_ = nullptr;
		}
	}
}
