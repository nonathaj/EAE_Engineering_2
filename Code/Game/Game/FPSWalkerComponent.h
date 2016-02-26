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

	float height() const { return height_; }
	void height(const float i_height) { height_ = i_height; }

	float groundable_check_length() const { return groundable_check_length_; }
	void groundable_check_length(const float i_gr_check_length) { groundable_check_length_ = i_gr_check_length; }

	std::shared_ptr<Lame::Physics3DComponent> physics_comp() const { return physics_comp_; }

	Lame::Vector3 FootPosition() const;

	void Update(float i_deltatime) override;

	void Enabled(bool enabled) override;

	Lame::Vector3 detached_pos() const { return pos; }
	Lame::Quaternion detached_rot() const { return rot; }

private:
	std::shared_ptr<Lame::Physics3DComponent> physics_comp_;

	float speed_;
	float rotation_rate_;
	float height_;
	float groundable_check_length_;

	//detach storage
	Lame::Vector3 pos;
	Lame::Quaternion rot;
};

#endif //_FPSWALKERCOMPONENT_H
