
#include "FPSWalkerComponent.h"

#include "../../Engine/Core/FloatMath.h"
#include "../../Engine/Core/Vector3.h"
#include "../../Engine/Physics/Physics3DComponent.h"
#include "../../Engine/Physics/Physics.h"
#include "../../Engine/System/UserInput.h"
#include "../../Engine/System/Console.h"
#include "../../Engine/Graphics/Graphics.h"

FPSWalkerComponent::FPSWalkerComponent(std::shared_ptr<Lame::Physics3DComponent> i_physics_comp) :
	IComponent(i_physics_comp->gameObject()),
	speed_(300.0f),
	rotation_rate_(60.0f),
	height_(120.0f),
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

void FindPlayerPosition(Lame::Vector3& io_position, const Lame::Vector3& i_world_dir, int index)
{
	using namespace Lame;

	if (i_world_dir == Vector3::zero || index <= 0)
		return;

	std::vector<Lame::Collision::RaycastHit> hits;
	if (LamePhysics::Get().RaycastAgainst(io_position, i_world_dir, hits))
	{
		int soonest_index = Lame::Collision::FindSoonestIndex(hits);
		if (soonest_index >= 0)
		{
			io_position = io_position + i_world_dir * hits[soonest_index].t + hits[soonest_index].normal * 10.0f;
			Vector3 dir = (i_world_dir * (1.0f - hits[soonest_index].t)).ProjectOnPlane(hits[soonest_index].normal);
			FindPlayerPosition(io_position, dir, index - 1);
		}
		else
			io_position += i_world_dir;
	}
	else
		io_position += i_world_dir;
}

void FPSWalkerComponent::Update(float i_deltatime)
{
	using namespace Lame;
	using namespace Lame::Input;

	std::shared_ptr<Lame::GameObject> go = gameObject();

	bool grounded = false;
	Lame::Collision::RaycastHit hitInfo;
	{
		Vector3 ray_start = go->transform().position();
		Vector3 ray_dir = Lame::Vector3::down * (groundable_check_length_ + height());
		std::vector<Lame::Collision::RaycastHit> grounded_hits;

		grounded = LamePhysics::Get().RaycastAgainst(ray_start, ray_dir, grounded_hits);
		if (grounded)
		{
			int soonest_index = Lame::Collision::FindSoonestIndex(grounded_hits);
			if (soonest_index >= 0)
			{
				hitInfo = grounded_hits[soonest_index];
			}
			else
				hitInfo.normal = Lame::Vector3::up;
		}
		else
			hitInfo.normal = Lame::Vector3::up;
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

	if (localMovement.sq_magnitude() > 0.0f)
	{
		Vector3 projectedMovement = gameObject()->transform().rotation() * localMovement.normalized();
		projectedMovement = projectedMovement.ProjectOnPlane(hitInfo.normal).normalized();
		Vector3 worldMovement = projectedMovement * (speed() * i_deltatime);

		//try to move the player
		Vector3 playerPos = FootPosition();
		FindPlayerPosition(playerPos, worldMovement, 2);
		go->transform().position(playerPos + Vector3::up * height());
	}

	go->transform().Rotate(Quaternion::Euler(localRotationAxis * rotation_rate_* i_deltatime));
}

void FPSWalkerComponent::Enabled(bool enabled)
{
	physics_comp_->enabled(enabled);
	Lame::Transform& t = gameObject()->transform();
	if (enabled)
	{
		t.position(pos);
		t.rotation(rot);
	}
	else
	{
		pos = t.position();
		rot = t.rotation();
	}
}
