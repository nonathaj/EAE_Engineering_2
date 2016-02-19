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

	float rotation_rate() const { return rotation_rate_; }
	void rotation_rate(const float i_rot_rate) { rotation_rate_ = i_rot_rate; }

	std::shared_ptr<Lame::Physics3DComponent> physics_comp() const { return physics_comp_; }

	void Update(float i_deltatime) override;

private:
	std::shared_ptr<Lame::Physics3DComponent> physics_comp_;

	float speed_;
	float rotation_rate_;
};

#endif //_FPSWALKERCOMPONENT_H
