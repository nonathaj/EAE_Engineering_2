
#include "World.h"

#include "GameObject.h"

namespace Engine
{
	World::World()
	{
	}


	World::~World()
	{
	}

	void World::Update(float deltaTime)
	{
		for (size_t x = 0; x < gameObjects_.size(); x++)
			gameObjects_[x]->Update(deltaTime);
	}

	bool World::Add(std::shared_ptr<GameObject> i_gameObject)
	{
		if (!Has(i_gameObject))
		{
			gameObjects_.push_back(i_gameObject);
			return true;
		}
		return false;
	}

	bool World::Has(std::shared_ptr<GameObject> i_gameObject)
	{
		return std::find(gameObjects_.begin(), gameObjects_.end(), i_gameObject) != gameObjects_.end();
	}

	bool World::Remove(std::shared_ptr<GameObject> i_gameObject)
	{
		auto goItr = std::find(gameObjects_.begin(), gameObjects_.end(), i_gameObject);
		if (goItr != gameObjects_.end())
		{
			gameObjects_.erase(goItr);
			return true;
		}
		return false;
	}
}