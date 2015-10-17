#ifndef _ENGINE_COMPONENT_ICOMPONENT_H
#define _ENGINE_COMPONENT_ICOMPONENT_H

#include <memory>

#include "../Core/TypeHandling.h"

namespace Engine
{
	class GameObject;

	class IComponent
	{
		ADD_TYPEID()
	public:
		inline virtual ~IComponent() {}

		//if the gameObject for this component is still valid, returns it, otherwise returns an empty shared pointer
		inline std::shared_ptr<GameObject> gameObject() { return gameObject_.expired() ? nullptr : gameObject_.lock(); }

		inline bool enabled() { return !gameObject_.expired() && enabled_; }
		inline void enabled(bool i_enabled) { enabled_ = i_enabled; }

	protected:
		//must be created by a child with this constructor
		IComponent(std::weak_ptr<GameObject> go);

	private:
		IComponent();
		bool enabled_;
		std::weak_ptr<GameObject> gameObject_;
	};
}

#endif //_ENGINE_COMPONENT_ICOMPONENT_H