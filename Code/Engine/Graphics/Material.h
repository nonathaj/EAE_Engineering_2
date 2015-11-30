#ifndef _ENGINE_LAME_MATERIAL_H
#define _ENGINE_LAME_MATERIAL_H

#include <vector>
#include <memory>

#include "../Core/HashedString.h"
#include "Effect.h"
#include "Texture.h"

namespace Lame
{
	class Material
	{
	public:
		struct Parameter
		{
			Texture *texture;
			Effect::ConstantHandle handle;
			float value[4];
			Effect::Shader shader_type;
			uint8_t valueCount;				//number of values to set
		};

		Material(const std::shared_ptr<Effect>& i_effect_) :effect_(i_effect_) {}
		static Material* Create(std::shared_ptr<Lame::Context> i_context, std::string i_path);

		~Material();

		bool Bind() const;

		std::shared_ptr<Effect> effect() const { return effect_; }

	private:
		std::shared_ptr<Effect> effect_;
		std::vector<Parameter> parameters_;
	};
}

#endif //_ENGINE_LAME_MATERIAL_H
