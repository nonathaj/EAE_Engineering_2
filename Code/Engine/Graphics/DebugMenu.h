#ifndef _LAME_DEBUGMENU_H
#define _LAME_DEBUGMENU_H

#include <memory>
#include <sstream>
#include <functional>
#include <vector>

#include "FontRenderer.h"

#ifdef _DEBUG
#define ENABLE_DEBUG_MENU
#endif

namespace Lame
{
	class Context;

	namespace Debug
	{
		struct Widget
		{
			std::string name;
			virtual void stream(std::stringstream& io_stream, const size_t i_width) const = 0;
			virtual void input(bool isPositive) = 0;
			Widget(std::string i_name) : name(i_name) {}
		};

		struct Text : Widget
		{
			std::string value;
			void stream(std::stringstream& io_stream, const size_t i_width) const override;
			void input(bool isPositive) override {}
			Text(std::string i_name, std::string i_val) :Widget(i_name), value(i_val) {}
		};

		struct Button : Widget
		{
			std::function<void()> callback;
			void stream(std::stringstream& io_stream, const size_t i_width) const override;
			void input(bool isPositive) override;
			Button(std::string i_name, std::function<void()> i_callback) : Widget(i_name), callback(i_callback) {}
		};

		struct CheckBox : Widget
		{
			bool* value;
			void stream(std::stringstream& io_stream, const size_t i_width) const override;
			void input(bool isPositive) override;
			CheckBox(std::string i_name, bool* i_val) : Widget(i_name), value(i_val) {}
		};

		struct Slider : Widget
		{
			float* value;
			float minimum;
			float maximum;
			void stream(std::stringstream& io_stream, const size_t i_width) const override;
			void input(bool isPositive) override;
			Slider(std::string i_name, float* i_val, float i_min, float i_max) : Widget(i_name), value(i_val), minimum(i_min), maximum(i_max) { }
		};

		class Menu
		{
		public:
			static Menu* Create(std::shared_ptr<Context> i_context);

			void CreateSlider(const char* name, float* value, float min, float max);
			void CreateCheckBox(const char* name, bool* value);
			void CreateText(const char* name, char* value);
			void CreateButton(const char* name, void(*callback)(void*), void *param);

			bool RenderAndUpdate();

			inline std::shared_ptr<FontRenderer> font_renderer() const { return font_renderer_; }
			inline void font_renderer(std::shared_ptr<FontRenderer> i_fr) { font_renderer_ = i_fr; }

			inline size_t width() const { return width_; }
			inline void width(size_t i_width) { width_ = i_width; }

			inline bool enabled() const { return enabled_; }
			inline void enabled(bool i_enabled) { enabled_ = i_enabled; }
		private:
			void Input();

			bool enabled_;
			size_t selected_widget_;
			size_t width_;
			std::vector<Widget> widgets;
			std::shared_ptr<FontRenderer> font_renderer_;
		};
	}
}

#endif //_LAME_DEBUGMENU_H
