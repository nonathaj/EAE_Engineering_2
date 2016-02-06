#ifndef _ENGINE_WORLD_H
#define _ENGINE_WORLD_H

#include <vector>
#include <memory>

#include "../Core/Singleton.h"

namespace Lame
{
	class GameObject;

	class World
	{
	public:
		~World();

		bool Setup();

		void Update(float deltaTime);

		bool Add(std::shared_ptr<GameObject> i_gameObject);
		bool Has(std::shared_ptr<GameObject> i_gameObject);
		bool Remove(std::shared_ptr<GameObject> i_gameObject);

		std::shared_ptr<GameObject> AddNewGameObject();			//returns a new gameobject inside this world

	private:
		World();

		std::vector<std::shared_ptr<GameObject>> gameObjects_;

		friend Lame::Singleton<Lame::World>;
	};
}

typedef Lame::Singleton<Lame::World> LameWorld;

#endif //_ENGINE_WORLD_H
