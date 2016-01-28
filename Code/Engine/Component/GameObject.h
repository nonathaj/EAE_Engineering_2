#ifndef _ENGINE_COMPONENT_GAMEOBJECT_H
#define _ENGINE_COMPONENT_GAMEOBJECT_H

#include <string>
#include <vector>
#include <memory>

#include "Transform.h"

namespace Engine
{
	class IComponent;

	class GameObject
	{
	public:
		GameObject();

		void Update(float deltaTime);
		
		inline std::string name() const { return name_; }
		inline void name(const std::string& i_name) { name_ = i_name; }
		
		inline bool enabled() const { return enabled_; }
		inline void enabled(const bool& i_enabled) { enabled_ = i_enabled; }

		inline void Destroy() { destroying_ = true; }
		inline bool IsDestroying() { return destroying_; }

		inline Engine::Transform& transform() { return transform_; }

	private:
		Engine::Transform transform_;
		std::string name_;

		bool enabled_;
		bool destroying_;

		std::vector<IComponent*> components_;

		friend class IComponent;
	};
}

#endif //_ENGINE_COMPONENT_GAMEOBJECT_H
