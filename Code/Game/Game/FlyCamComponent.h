#ifndef _FLY_CAM_COMPONENT_H
#define _FLY_CAM_COMPONENT_H

#include "../../Engine/Component/IComponent.h"

class FlyCamComponent : public Lame::IComponent
{
public:
	FlyCamComponent(std::shared_ptr<Lame::GameObject> go);

	void Update(float deltaTime) override;
	void Enabled(bool enabled) override;
private:

};

#endif //_FLY_CAM_COMPONENT_H
