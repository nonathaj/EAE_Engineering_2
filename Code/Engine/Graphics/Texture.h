#ifndef _ENGINE_LAME_TEXTURE_H
#define _ENGINE_LAME_TEXTURE_H

#include <memory>
#include <string>

#if EAE6320_PLATFORM_D3D
#include <d3d9.h>
#elif EAE6320_PLATFORM_GL
#include "../../Engine/Windows/Includes.h"
#include <gl/GL.h>
#endif

namespace Lame
{
	class Context;

	class Texture
	{
	public:
		static Texture* Create(std::shared_ptr<Context> i_context, const std::string& i_path);

		~Texture();

#if EAE6320_PLATFORM_D3D
		IDirect3DTexture9* texture() const { return texture_; }
#elif EAE6320_PLATFORM_GL
		GLuint texture_id() const { return texture_id_; }
#endif
	private:
		//Do not allow Textures to be managed without pointers
		Texture() {}
		Texture(const Texture &i_mesh);
		Texture& operator=(const Texture &i_mesh);

#if EAE6320_PLATFORM_D3D
		IDirect3DTexture9 *texture_;
#elif EAE6320_PLATFORM_GL
		GLuint texture_id_;
#else
#error No definition to store a texture
#endif
	};
}

#endif //_ENGINE_LAME_TEXTURE_H
