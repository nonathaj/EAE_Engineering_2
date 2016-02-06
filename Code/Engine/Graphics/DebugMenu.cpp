
#include "DebugMenu.h"

#include "../Core/Vector2.h"
#include "../Core/Rectangle2D.h"
#include "../System/UserInput.h"
#include "../System/Console.h"

namespace Lame
{
	namespace Debug
	{
		void Widget::stream(std::stringstream& io_stream, const size_t i_width, const bool i_selected) const
		{
			if (i_selected)
				io_stream << " -> ";
			else
				io_stream << "    ";
			io_stream << name << " ";
			if (i_width > 0 && name.size() > 0)
				io_stream << std::string(i_width - name.size() - 1, '-');
			io_stream << std::endl << "    ";
		}

		void Text::stream(std::stringstream& io_stream, const size_t i_width, const bool i_selected) const
		{
			Widget::stream(io_stream, i_width, i_selected);
			io_stream << value << std::endl;
		}

		void Button::stream(std::stringstream& io_stream, const size_t i_width, const bool i_selected) const
		{
			Widget::stream(io_stream, i_width, i_selected);
			io_stream << "( Press to activate ) " << std::endl;
		}

		void Button::input(bool isPositive)
		{
			callback();
		}

		void CheckBox::stream(std::stringstream& io_stream, const size_t i_width, const bool i_selected) const
		{
			Widget::stream(io_stream, i_width, i_selected);
			io_stream << '[' << (*value ? 'x' : ' ') << ']' << std::endl;
		}

		void CheckBox::input(bool isPositive)
		{
			*value = isPositive;
		}

		void Slider::stream(std::stringstream& io_stream, const size_t i_width, const bool i_selected) const
		{
			Widget::stream(io_stream, i_width, i_selected);
			io_stream << '[';
			{
				size_t max_positions = i_width - 2;
				size_t current_position = static_cast<size_t>((*value - minimum) / (maximum - minimum) * max_positions);
				if(current_position > 1)
					io_stream << std::string(current_position - 1, '=');
				io_stream << '|';
				if(max_positions > current_position && (max_positions - current_position) > 0)
					io_stream << std::string(max_positions - current_position, ' ');
			}
			io_stream << ']' << std::endl;
		}

		void Slider::input(bool isPositive)
		{
			const float change_percent = 0.05f;
			const float change_amt = (maximum - minimum) * (isPositive ? 1.f : -1.f) * change_percent;
			*value += change_amt;
			if (*value < minimum)
				*value = minimum;
			if (*value > maximum)
				*value = maximum;
		}

		Menu* Menu::Create(std::shared_ptr<Context> i_context)
		{
			if (!i_context)
				return nullptr;

			std::shared_ptr<FontRenderer> fr(FontRenderer::Create(i_context, 
				Vector2(0.01f, 0.03f), 
				Font::Pitch::Monospace, 
				Font::Type::Normal, 
				"Consolas") );
			if (!fr)
				return nullptr;

			Menu* m = new Menu();
			if (m)
			{
				m->font_renderer_ = fr;
				m->width_ = 40;
			}
			return m;
		}

		void Menu::Input()
		{
			using namespace Lame::Input;

			if (LameInput::Get().DownRaw(Keyboard::Tilde))
			{
				enabled_ = !enabled_;
			}
			LameInput::Get().enabled(!enabled_);

			if (!enabled_ || widgets.size() == 0)
				return;


			if (LameInput::Get().DownRaw(Keyboard::Down))
			{
				selected_widget_++;
				if (selected_widget_ == widgets.size())
					selected_widget_ = 0;
			}
			if (LameInput::Get().DownRaw(Keyboard::Up))
			{
				if (selected_widget_ == 0)
					selected_widget_ = widgets.size() - 1;
				else
					selected_widget_--;
			}
			if (LameInput::Get().DownRaw(Keyboard::Left))
				widgets[selected_widget_]->input(false);
			if(LameInput::Get().DownRaw(Keyboard::Right))
				widgets[selected_widget_]->input(true);
		}

		Menu::~Menu()
		{
			for (auto itr = widgets.begin(); itr != widgets.end(); ++itr)
			{
				delete *itr;
			}
			widgets.clear();
		}

		bool Menu::RenderAndUpdate()
		{
			Input();

			if (widgets.size() == 0 || !enabled_)
				return true;

			std::stringstream content;
			for (size_t x = 0; x < widgets.size(); x++)
			{
				widgets[x]->stream(content, width_, x == selected_widget_);
			}

			return font_renderer()->Render(
				content.str().c_str(),
				Rectangle2D::CreateBLNormalized(),
				Font::HorizontalAlignment::Left,
				false,
				Color32::green);
		}

		void Menu::CreateSlider(const char* name, float* value, float min, float max)
		{
			widgets.push_back(new Slider(name, value, min, max));
		}

		void Menu::CreateCheckBox(const char* name, bool* value)
		{
			widgets.push_back(new CheckBox(name, value));
		}

		void Menu::CreateText(const char* name, char* value)
		{
			widgets.push_back(new Text(name, value));
		}

		void Menu::CreateButton(const char* name, std::function<void()> i_function)
		{
			widgets.push_back(new Button(name, i_function));
		}
	}
}