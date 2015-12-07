
#include "Vector3.h"
#include "Vector2.h"

const Engine::Vector3 Engine::Vector3::zero = Vector3(0, 0, 0);
const Engine::Vector3 Engine::Vector3::one = Vector3(1, 1, 1);
const Engine::Vector3 Engine::Vector3::up = Vector3(0, 1, 0);
const Engine::Vector3 Engine::Vector3::down = Vector3(0, -1, 0);
const Engine::Vector3 Engine::Vector3::left = Vector3(-1, 0, 0);
const Engine::Vector3 Engine::Vector3::right = Vector3(1, 0, 0);
const Engine::Vector3 Engine::Vector3::forward = Vector3(0, 0, -1);
const Engine::Vector3 Engine::Vector3::back = Vector3(0, 0, 1);

Engine::Vector3::Vector3(const Vector2 &i_vec, float i_z)
	:m_x(i_vec.x()), m_y(i_vec.y()), m_z(i_z)
{
}
