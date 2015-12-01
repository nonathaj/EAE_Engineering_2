#ifndef _ENGINE_WORLD_H
#define _ENGINE_WORLD_H

#include <vector>
#include <memory>

namespace Engine
{
	class GameObject;

	class World
	{
	public:
		World();
		~World();

		void Update(float deltaTime);

		bool Add(std::shared_ptr<GameObject> i_gameObject);
		bool Has(std::shared_ptr<GameObject> i_gameObject);
		bool Remove(std::shared_ptr<GameObject> i_gameObject);

	private:
		std::vector<std::shared_ptr<GameObject>> gameObjects_;
	};
}

#endif //_ENGINE_WORLD_H