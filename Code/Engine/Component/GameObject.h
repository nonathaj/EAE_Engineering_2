#ifndef _ENGINE_COMPONENT_GAMEOBJECT_H
#define _ENGINE_COMPONENT_GAMEOBJECT_H

#include <string>
#include <vector>

#include "../Core/Vector3.h"

namespace Engine
{
	class IComponent;

	class GameObject
	{
	public:
		GameObject();

		inline Vector3 position() const { return position_; }
		inline void position(const Vector3& i_pos) { position_ = i_pos; }

		inline std::string name() const { return name_; }
		inline void name(const std::string& i_name) { name_ = i_name; }
		
		inline bool enabled() const { return enabled_; }
		inline void enabled(const bool& i_enabled) { enabled_ = i_enabled; }

	private:
		Vector3 position_;
		std::string name_;

		bool enabled_;

		std::vector<IComponent> components_;
	};
}

#endif //_ENGINE_COMPONENT_GAMEOBJECT_H