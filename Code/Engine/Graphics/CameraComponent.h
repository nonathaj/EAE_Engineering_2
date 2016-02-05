#ifndef _ENGINE_GRAPHICS_CAMERACOMPONENT_H
#define _ENGINE_GRAPHICS_CAMERACOMPONENT_H

#include "../Component/IComponent.h"
#include "../Component/GameObject.h"
#include "Context.h"
#include "../Core/Matrix4x4.h"

namespace Lame
{
	class CameraComponent : public Lame::IComponent
	{
		ADD_TYPEID()
	public:
		CameraComponent(std::weak_ptr<Lame::GameObject> go, std::shared_ptr<Context> contextPtr, float i_vertical_fov_degree = 60.0f, float i_near_clip_plane = 0.1f, float i_far_clip_plane = 100.0f);

		Lame::Matrix4x4 WorldToView() const;
		Lame::Matrix4x4 ViewToScreen() const;

		float near_clip_plane() const { return near_clip_plane_; }
		void near_clip_plane(const float& i_near_clip_plane) { near_clip_plane_ = i_near_clip_plane; }

		float far_clip_plane() const { return far_clip_plane_; }
		void far_clip_plane(const float& i_far_clip_plane) { far_clip_plane_ = i_far_clip_plane; }

		//in degrees
		float vertical_fov() const { return vertical_fov_degrees_; }
		void vertical_fov(const float& i_vertical_fov_degrees) { vertical_fov_degrees_ = i_vertical_fov_degrees; }

		float aspect_ratio() const;

		inline std::shared_ptr<Context> context() const { return context_; }
	private:
		CameraComponent();

		float vertical_fov_degrees_;
		float near_clip_plane_; 
		float far_clip_plane_;

		std::shared_ptr<Context> context_;
	};
}

#endif //#define _ENGINE_GRAPHICS_CAMERACOMPONENT_H

