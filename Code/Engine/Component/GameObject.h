#ifndef _ENGINE_COMPONENT_GAMEOBJECT_H
#define _ENGINE_COMPONENT_GAMEOBJECT_H

#include <string>
#include <vector>
#include <memory>

#include "../Core/Vector3.h"
#include "../Core/Quaternion.h"
#include "../Core/eae6320/cMatrix_transformation.h"

namespace Engine
{
	class IComponent;

	class GameObject
	{
	public:
		GameObject();

		void Move(const Engine::Vector3& i_movement);
		void Rotate(const Engine::Quaternion& i_rotation);

		void Update(float deltaTime);
		
		inline Engine::Vector3 position() const { return position_; }
		inline void position(const Engine::Vector3& i_pos) { position_ = i_pos; }

		Engine::Quaternion rotation() const { return rotation_; }
		void rotation(const Engine::Quaternion& i_rotation) { rotation_ = i_rotation; }

		inline std::string name() const { return name_; }
		inline void name(const std::string& i_name) { name_ = i_name; }
		
		inline bool enabled() const { return enabled_; }
		inline void enabled(const bool& i_enabled) { enabled_ = i_enabled; }

		inline eae6320::Math::cMatrix_transformation LocalToWorld() const { return eae6320::Math::cMatrix_transformation(rotation(), position()); }

	private:
		Engine::Vector3 position_;
		Engine::Quaternion rotation_;
		std::string name_;

		bool enabled_;

		std::vector<IComponent*> components_;

		friend class IComponent;
	};
}

#endif //_ENGINE_COMPONENT_GAMEOBJECT_H
