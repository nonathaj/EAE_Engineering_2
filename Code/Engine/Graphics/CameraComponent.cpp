
#include "CameraComponent.h"

#include "Context.h"
#include "../Core/Math.h"
#include "../Core/Matrix4x4.h"

namespace Lame
{
	CameraComponent::CameraComponent(std::weak_ptr<Lame::GameObject> go, std::shared_ptr<Context> contextPtr, float i_vertical_fov_degree, float i_near_clip_plane, float i_far_clip_plane) :
		IComponent(go), 
		context_(contextPtr), 
		camera_type_(Type::Perspective),
		vertical_size_(i_vertical_fov_degree),
		near_clip_plane_(i_near_clip_plane), 
		far_clip_plane_(i_far_clip_plane)
	{
	}

	Lame::Matrix4x4 CameraComponent::WorldToView() const
	{ 
		return Lame::Matrix4x4::CreateWorldToView(gameObject()->transform().position(), gameObject()->transform().rotation());
	}

	Lame::Matrix4x4 CameraComponent::ViewToScreen() const
	{
		switch (camera_type_)
		{
		case Type::Orthographic:
			return Matrix4x4::CreateOrthographicViewToScreen(
				vertical_size_, context()->aspect_ratio() * vertical_size_, near_clip_plane_, far_clip_plane_);
		case Type::Perspective:
			return Matrix4x4::CreatePerspectiveViewToScreen(
				static_cast<float>(Lame::Math::ToRadians(vertical_size_)), aspect_ratio(), near_clip_plane_, far_clip_plane_);
		default:
			//return an invalid matrix
			return Lame::Matrix4x4();
		}
	}

	float CameraComponent::aspect_ratio() const
	{ 
		return context()->aspect_ratio();
	}
}
