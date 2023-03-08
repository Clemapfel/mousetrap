//
// Created by clem on 3/8/23.
//

#include <app/log_box.hpp>
#include <app/project_state.hpp>

namespace mousetrap
{
    LogBox::LogBox()
    {
        _main.set_margin(state::margin_unit * 0.5);
        _main.push_back(&_current_save_path_label);
        _main.push_back(&_spacer);
        _main.push_back(&_cursor_position_label);

        _spacer.set_opacity(0);
        _spacer.set_hexpand(true);

        _current_save_path_label.set_halign(GTK_ALIGN_START);
        _cursor_position_label.set_halign(GTK_ALIGN_END);

        on_cursor_position_changed();
        on_save_path_changed();
    }

    void LogBox::on_cursor_position_changed()
    {
        set_cursor_position(active_state->get_cursor_position());
    }

    void LogBox::on_save_path_changed()
    {
        set_save_path(active_state->get_save_path());
    }

    void LogBox::set_cursor_position(Vector2i position)
    {
        std::string x_sign = (position.x < 0 ? "-" : "");
        std::string y_sign = (position.y < 0 ? "-" : "");

        auto x_value = abs(position.x);
        auto y_value = abs(position.y);

        std::stringstream str;
        //str << "(" << "<tt>" << x_sign << (x_value < 10 ? "0" : "") << x_value << "</tt>"
        //    << " " << "<tt>" << y_sign << (y_value < 10 ? "0" : "") << y_value << "</tt>)";

        str << "( " << "<tt>" << x_value << "</tt>"
            << " | " << "<tt>" << y_value << "</tt> )";

        _cursor_position_label.set_text(str.str());
    }

    void LogBox::set_save_path(const std::string& path)
    {
        std::deque<char> str;

        size_t folder_count = 0;
        for (int i = path.size() - 1; not path.empty() and i >= 0; i--)
        {
            if (path.at(i) == '/' or path.at(i) == '\\')
            {
                folder_count += 1;
                if (folder_count >= 2)
                    break;
            }

            str.push_front(path.at(i));
        }

        std::string label;
        label.reserve(str.size());
        for (auto c : str)
        {
            if (c == '/' or c == '\\')
            {
                label.push_back(' ');
                label.push_back('>');
                label.push_back(' ');
            }
            else
                label.push_back(c);
        }

        _current_save_path_label.set_text(label);
    }

    LogBox::operator Widget*()
    {
        return &_main;
    }
}
