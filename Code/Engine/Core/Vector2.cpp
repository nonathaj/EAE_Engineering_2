#include "Vector2.h"
#include "Vector3.h"

const Engine::Vector2 Engine::Vector2::zero = Vector2(0, 0);
const Engine::Vector2 Engine::Vector2::one = Vector2(1, 1);
const Engine::Vector2 Engine::Vector2::up = Vector2(0, 1);
const Engine::Vector2 Engine::Vector2::down = Vector2(0, -1);
const Engine::Vector2 Engine::Vector2::left = Vector2(-1, 0);
const Engine::Vector2 Engine::Vector2::right = Vector2(1, 0);

Engine::Vector2::Vector2(const Vector3 &i_vec)
	:m_x(i_vec.x()), m_y(i_vec.y())
{
}