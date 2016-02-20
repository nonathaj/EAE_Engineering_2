
#include "FPSWalkerComponent.h"

#include "../../Engine/Core/FloatMath.h"
#include "../../Engine/Core/Vector3.h"
#include "../../Engine/Physics/Physics3DComponent.h"
#include "../../Engine/Physics/Physics.h"
#include "../../Engine/System/UserInput.h"
#include "../../Engine/System/Console.h"

FPSWalkerComponent::FPSWalkerComponent(std::shared_ptr<Lame::Physics3DComponent> i_physics_comp) :
	IComponent(i_physics_comp->gameObject()),
	speed_(300.0f),
	rotation_rate_(60.0f),
	height_(150.0f),
	groundable_check_length_(23.0f),
	physics_comp_(i_physics_comp)
{
}

Lame::Vector3 FPSWalkerComponent::FootPosition() const
{
	auto go = gameObject();
	if (!go)
	{
		float nan = Lame::Math::Float::NaN();
		return Lame::Vector3(nan, nan, nan);
	}
	return gameObject()->transform().position() + Lame::Vector3::down * height();
}

void FPSWalkerComponent::Update(float i_deltatime)
{
	using namespace Lame;
	using namespace Lame::Input;

	std::vector<Lame::Collision::RaycastHit> grounded_hits;
	bool grounded = LamePhysics::Get().RaycastAgainst(FootPosition(), Lame::Vector3::down * groundable_check_length_, grounded_hits);
	if (grounded || grounded_hits.size() > 0)
	{
		DEBUG_PRINT("grounded=%s hits=%d", grounded ? "yes" : "no", grounded_hits.size());
	}

	if (grounded)
	{
		physics_comp_->gravity_multiplier(0.0f);
		physics_comp_->velocity(Vector3::zero);
	}
	else
		physics_comp_->gravity_multiplier(1.0f);

	Vector3 localMovement = Vector3::zero;
	Vector3 localRotationAxis = Vector3::zero;
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

		if (LameInput::Get().Down(Keyboard::Space))
			physics_comp_->velocity(physics_comp_->velocity() + Vector3::up * 10.0f);
	}

	std::shared_ptr<Lame::GameObject> go = gameObject();
	if (localMovement.sq_magnitude() > 0.0f)
	{
		Vector3 worldMovement = gameObject()->transform().rotation() * localMovement.normalized() * speed() * i_deltatime;

		go->transform().Move(worldMovement);
	}
	go->transform().Rotate(Quaternion::Euler(localRotationAxis * rotation_rate_* i_deltatime));
}
