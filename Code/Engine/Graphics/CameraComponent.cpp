
#include "CameraComponent.h"

#include "Context.h"
#include "../Core/Math.h"

namespace Lame
{
	CameraComponent::CameraComponent(std::weak_ptr<Engine::GameObject> go, std::shared_ptr<Context> contextPtr, float i_vertical_fov_degree, float i_near_clip_plane, float i_far_clip_plane) :
		IComponent(go), 
		context_(contextPtr), 
		vertical_fov_degrees_(i_vertical_fov_degree), 
		near_clip_plane_(i_near_clip_plane), 
		far_clip_plane_(i_far_clip_plane)
	{
	}
	eae6320::Math::cMatrix_transformation CameraComponent::WorldToView() const
	{ 
		return eae6320::Math::cMatrix_transformation::CreateWorldToViewTransform(gameObject()->rotation(), gameObject()->position());
	}

	eae6320::Math::cMatrix_transformation CameraComponent::ViewToScreen() const
	{
		return eae6320::Math::cMatrix_transformation::CreateViewToScreenTransform(
			static_cast<float>(Engine::Math::ToRadians(vertical_fov_degrees_)), 
			aspect_ratio(), near_clip_plane_, far_clip_plane_);
	}

	float CameraComponent::aspect_ratio() const
	{ 
		return static_cast<float>(context()->screen_width()) / static_cast<float>(context()->screen_height()); 
	}
}
