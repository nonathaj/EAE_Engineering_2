
#include "Material.h"

namespace Lame
{
	bool Material::Bind() const
	{
		bool success = effect()->Bind();

		return success;
	}
}
