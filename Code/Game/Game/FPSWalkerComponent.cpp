
#include "FPSWalkerComponent.h"

#include "../../Engine/Physics/Physics3DComponent.h"

FPSWalkerComponent::FPSWalkerComponent(std::shared_ptr<Lame::Physics3DComponent> i_physics_comp) :
	IComponent(i_physics_comp->gameObject()),
	speed_(50.0f)
{
}

void FPSWalkerComponent::Update(float i_deltatime)
{

}
