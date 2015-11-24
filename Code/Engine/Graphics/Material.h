#ifndef _LAME_MATERIAL_H
#define _LAME_MATERIAL_H

#include <memory>

#include "../Core/HashedString.h"
#include "Effect.h"

namespace Lame
{
	class Material
	{
	public:
		struct Parameter
		{
			float value[4];
			Effect::ConstantHandle handle;
			uint8_t valueCount;				//number of values to set
			Effect::Shader shader_type;
		};

		Material(const std::shared_ptr<Effect>& i_effect_) :effect_(i_effect_) {}
		static Material* Create(std::shared_ptr<Lame::Context> i_context, std::string i_path);

		bool Bind() const;

		std::shared_ptr<Effect> effect() const { return effect_; }

	private:
		std::shared_ptr<Effect> effect_;
		std::vector<Parameter> parameters_;
	};
}

#endif //_LAME_MATERIAL_H
