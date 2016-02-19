
#include "FPSWalkerComponent.h"

#include "../../Engine/Core/FloatMath.h"
#include "../../Engine/Core/Vector3.h"
#include "../../Engine/Physics/Physics3DComponent.h"
#include "../../Engine/System/UserInput.h"

FPSWalkerComponent::FPSWalkerComponent(std::shared_ptr<Lame::Physics3DComponent> i_physics_comp) :
	IComponent(i_physics_comp->gameObject()),
	speed_(300.0f),
	rotation_rate_(60.0f)
{
}

void FPSWalkerComponent::Update(float i_deltatime)
{
	using namespace Lame;
	using namespace Lame::Input;

	Vector3 localMovement = Vector3::one;
	Vector3 localRotationAxis = Vector3::one;
	if (LameInput::Exists())
	{
		if (LameInput::Get().Held(Keyboard::A))
			localMovement += Vector3::left;
		if (LameInput::Get().Held(Keyboard::D))
			localMovement += Vector3::right;
		if (LameInput::Get().Held(Keyboard::W))
			localMovement += Vector3::forward;
		if (LameInput::Get().Held(Keyboard::S))
			localMovement += Vector3::back;

		if (LameInput::Get().Held(Keyboard::Left))						//rotate left
			localRotationAxis += Vector3::up;
		if (LameInput::Get().Held(Keyboard::Right))						//rotate Right
			localRotationAxis += Vector3::down;
	}
	
	if (localMovement.sq_magnitude() > 0.0f)
	{
		Vector3 worldMovement = gameObject()->transform().rotation() * localMovement.normalized() * speed();
		//todo move player
	}
}
