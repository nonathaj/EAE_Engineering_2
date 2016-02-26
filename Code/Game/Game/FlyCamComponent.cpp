#include "FlyCamComponent.h"

#include "../../Engine/Core/Vector3.h"
#include "../../Engine/System/UserInput.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/System/Console.h"

FlyCamComponent::FlyCamComponent(std::shared_ptr<Lame::GameObject> go) :
	IComponent(go)
{

}

void FlyCamComponent::Update(float deltaTime)
{
	using namespace Lame;
	using namespace Lame::Input;

	const float movementAmount = 300.0f * deltaTime;
	std::shared_ptr<GameObject> movableObject = LameGraphics::Get().camera()->gameObject();
	Vector3 movementVector = Vector3::zero;
	if (LameInput::Get().Held(Keyboard::W))						//forward
		movementVector += Vector3::forward;
	if (LameInput::Get().Held(Keyboard::S))						//backward
		movementVector += Vector3::back;
	if (LameInput::Get().Held(Keyboard::D))						//right
		movementVector += Vector3::right;
	if (LameInput::Get().Held(Keyboard::A))						//left
		movementVector += Vector3::left;
	if (LameInput::Get().Held(Keyboard::E))						//up
		movementVector += Vector3::up;
	if (LameInput::Get().Held(Keyboard::Q))						//down
		movementVector += Vector3::down;

	movementVector = LameGraphics::Get().camera()->gameObject()->transform().rotation() * movementVector * movementAmount;
	if (!movementVector.AnyNaN())
	{
		LameGraphics::Get().camera()->gameObject()->transform().Move(movementVector);
	}
	else
	{
		DEBUG_PRINT("Invalid movement vector");
	}

	const float rotationAmount = 40.0f * deltaTime;
	Vector3 rotationAxis = Vector3::zero;
	if (LameInput::Get().Held(Keyboard::Left))						//rotate left
		rotationAxis += Vector3::up;
	if (LameInput::Get().Held(Keyboard::Right))						//rotate Right
		rotationAxis += Vector3::down;
	//if (Keyboard::Pressed(Keyboard::Up))						//rotate up
	//	rotationAxis += Vector3::Vector3::right;
	//if (Keyboard::Pressed(Keyboard::Down))						//rotate Down
	//	rotationAxis += Vector3::Vector3::left;

	movableObject->transform().Rotate(Quaternion::Euler(rotationAxis * rotationAmount));
}

void FlyCamComponent::Enabled(bool enabled)
{

}
