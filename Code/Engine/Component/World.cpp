
#include "World.h"

#include "GameObject.h"

namespace Lame
{
	World::World()
	{
	}


	World::~World()
	{
	}

	bool World::Setup()
	{
		return true;
	}

	void World::Update(float deltaTime)
	{
		for (auto itr = gameObjects_.begin(); itr != gameObjects_.end(); /**/)
		{
			if ((*itr)->IsDestroying())
			{
				itr = gameObjects_.erase(itr);
			}
			else
			{
				(*itr)->Update(deltaTime);
				++itr;
			}
		}
	}

	void World::PhysicsUpdate(float deltaTime)
	{
		for (auto itr = gameObjects_.begin(); itr != gameObjects_.end(); /**/)
		{
			if ((*itr)->IsDestroying())
			{
				itr = gameObjects_.erase(itr);
			}
			else
			{
				(*itr)->PhysicsUpdate(deltaTime);
				++itr;
			}
		}
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

	std::shared_ptr<GameObject> World::AddNewGameObject()
	{
		std::shared_ptr<GameObject> go(new GameObject());
		if (!go)
			return nullptr;

		if (!Add(go))
			return nullptr;

		return go;
	}
}
