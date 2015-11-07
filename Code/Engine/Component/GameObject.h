#ifndef _ENGINE_COMPONENT_GAMEOBJECT_H
#define _ENGINE_COMPONENT_GAMEOBJECT_H

#include <string>
#include <vector>

#include "../Core/eae6320/cVector.h"
#include "../Core/eae6320/cQuaternion.h"

namespace Engine
{
	class IComponent;

	class GameObject
	{
	public:
		GameObject();

		void Move(const eae6320::Math::cVector& i_movement);

		inline eae6320::Math::cVector position() const { return position_; }
		inline void position(const eae6320::Math::cVector& i_pos) { position_ = i_pos; }

		eae6320::Math::cQuaternion rotation() { return rotation_; }
		void rotation(const eae6320::Math::cQuaternion& i_rotation) { rotation_ = i_rotation; }

		inline std::string name() const { return name_; }
		inline void name(const std::string& i_name) { name_ = i_name; }
		
		inline bool enabled() const { return enabled_; }
		inline void enabled(const bool& i_enabled) { enabled_ = i_enabled; }

	private:
		eae6320::Math::cVector position_;
		eae6320::Math::cQuaternion rotation_;
		std::string name_;

		bool enabled_;

		std::vector<IComponent> components_;
	};
}

#endif //_ENGINE_COMPONENT_GAMEOBJECT_H
