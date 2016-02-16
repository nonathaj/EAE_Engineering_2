
#include "Physics3DComponent.h"

#include "../Component/GameObject.h"
#include "../Component/Transform.h"
#include "Physics.h"

namespace Lame
{
	Physics3DComponent::Physics3DComponent(std::weak_ptr<GameObject> go) :
		IComponent(go),
		gravity_multiplier_(1.0f),
		velocity_(Vector3::zero),
		constant_acceleration_(Vector3::zero)
	{
	}
}
