
#include "DebugMenu.h"

#include "../Core/Vector2.h"
#include "../Core/Rectangle2D.h"
#include "../System/UserInput.h"
#include "../System/Console.h"

namespace Lame
{
	namespace Debug
	{
		void Text::stream(std::stringstream& io_stream, const size_t i_width) const
		{
			io_stream << name;
			if (name.size() + value.size() + 3 > i_width)
				io_stream << " " << std::string(i_width - name.size() - 1, '-') << std::endl;
			else
				io_stream << " : ";
			io_stream << value << std::endl;
		}

		void Button::stream(std::stringstream& io_stream, const size_t i_width) const
		{
			io_stream << name << std::endl;
		}

		void Button::input(bool isPositive)
		{
			callback();
		}

		void CheckBox::stream(std::stringstream& io_stream, const size_t i_width) const
		{
			io_stream << name;
			if (name.size() + 4 > i_width)
				io_stream << " " << std::string(i_width - name.size() - 1, '-') << std::endl;

			io_stream << " [" << (*value ? 'x' : ' ') << ']' << std::endl;
		}

		void CheckBox::input(bool isPositive)
		{
			*value = isPositive;
		}

		void Slider::stream(std::stringstream& io_stream, const size_t i_width) const
		{
			io_stream << name << " " << std::string(i_width - name.size() - 1, '-') << std::endl << '[';
			{
				size_t max_positions = i_width - 2;
				size_t current_position = static_cast<size_t>((*value - minimum) / (maximum - minimum) * max_positions);
				io_stream << std::string(current_position - 1, '=') << '|' << std::string(max_positions - current_position, ' ');
			}
			io_stream << ']' << std::endl;
		}

		void Slider::input(bool isPositive)
		{
			const float change_percent = 0.05f;
			const float change_amt = (maximum - minimum) * (isPositive ? 1.f : -1.f) * change_percent;
			*value += change_amt;
		}

		Menu* Menu::Create(std::shared_ptr<Context> i_context)
		{
			if (!i_context)
				return nullptr;

			std::shared_ptr<FontRenderer> fr(FontRenderer::Create(i_context, Vector2(0.01f, 0.03f), Font::Pitch::Monospace, Font::Type::Normal, "Consolas"));
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

			DEBUG_PRINT("dm=%s input=%s", (enabled_ ? "y" : "n"), (LameInput::Get().enabled() ? "y" : "n"));

			if (!enabled_ || widgets.size() == 0)
				return;


			if (LameInput::Get().DownRaw(Keyboard::Up))
			{
				selected_widget_++;
				if (selected_widget_ == widgets.size())
					selected_widget_ = 0;
			}
			if (LameInput::Get().DownRaw(Keyboard::Down))
			{
				if (selected_widget_ == 0)
					selected_widget_ = widgets.size() - 1;
				else
					selected_widget_--;
			}
			if (LameInput::Get().DownRaw(Keyboard::Left))
				widgets[selected_widget_].input(false);
			if(LameInput::Get().DownRaw(Keyboard::Right))
				widgets[selected_widget_].input(true);
		}

		bool Menu::RenderAndUpdate()
		{
			Input();

			if (widgets.size() == 0 || !enabled_)
				return true;

			std::stringstream content;
			for (auto itr = widgets.begin(); itr != widgets.end(); ++itr)
				itr->stream(content, width_);

			return font_renderer()->Render(
				content.str().c_str(),
				Rectangle2D(0, 0, 1, 1),
				Font::HorizontalAlignment::Left,
				false,
				Color32::green);
		}
	}
}