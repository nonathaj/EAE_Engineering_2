#ifndef _FPSWALKERCOMPONENT_H
#define _FPSWALKERCOMPONENT_H

#include "../../Engine/Component/IComponent.h"
#include "../../Engine/Component/GameObject.h"

namespace Lame
{
	class Physics3DComponent;
}

class FPSWalkerComponent : public Lame::IComponent
{
public:
	FPSWalkerComponent(std::shared_ptr<Lame::Physics3DComponent> i_physics_comp);

	float speed() const { return speed_; }
	void speed(const float i_speed) { speed_ = i_speed; }

	std::shared_ptr<Lame::Physics3DComponent> physics_comp() const { return physics_comp_; }

	void Update(float i_deltatime) override;

private:
	std::shared_ptr<Lame::Physics3DComponent> physics_comp_;

	float speed_;
};

#endif //_FPSWALKERCOMPONENT_H
