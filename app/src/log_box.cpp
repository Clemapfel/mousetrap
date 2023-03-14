//
// Created by clem on 3/8/23.
//

#include <app/log_box.hpp>
#include <app/project_state.hpp>

namespace mousetrap
{
    LogBox::LogBox()
    {
        _main.push_back(&_current_save_path_label);
        _main.push_back(&_spacer);
        _main.push_back(&_current_color_label);
        _main.push_back(&_current_resolution_label);
        _main.push_back(&_cursor_position_label);

        _main.set_margin(state::margin_unit * 0.5);

        _spacer.set_opacity(0);
        _spacer.set_vexpand(false);
        _spacer.set_hexpand(true);

        for (auto* label : {
            &_current_save_path_label,
            &_cursor_position_label,
            &_current_color_label,
            &_current_resolution_label
        })
        {
            label->set_margin_horizontal(state::margin_unit);
            label->set_valign(GTK_ALIGN_CENTER);
        }

        on_cursor_position_changed();
        on_save_path_changed();
        on_layer_resolution_changed();

        auto tooltip = [](const std::string& value) {
            return state::tooltips_file->get_value("log_box", value);
        };

        _current_save_path_label.set_tooltip_text(tooltip("current_save_path"));
        _current_color_label.set_tooltip_text(tooltip("current_color"));
        _cursor_position_label.set_tooltip_text(tooltip("cursor_position"));
        _current_resolution_label.set_tooltip_text(tooltip("current_resolution"));
    }

    void LogBox::on_cursor_position_changed()
    {
        set_cursor_position(active_state->get_cursor_position());
        update_current_color();
    }

    void LogBox::on_layer_image_updated()
    {
        update_current_color();
    }

    void LogBox::on_layer_frame_selection_changed()
    {
        update_current_color();
    }

    void LogBox::on_save_path_changed()
    {
        set_save_path(active_state->get_save_path());
    }

    void LogBox::update_current_color()
    {
        auto pos = active_state->get_cursor_position();
        auto frame = active_state->get_frame(
            active_state->get_current_layer_index(),
            active_state->get_current_frame_index()
        );

        auto color_rgba = frame->get_pixel(pos.x, pos.y);

        if (pos.x < 0 or pos.y < 0 or pos.x >= frame->get_size().x or pos.y >= frame->get_size().y)
        {
            _current_color_label.set_text("HSVA(<tt>?</tt>, <tt>?</tt>, <tt>?</tt>, <tt>?</tt>)");
            return;
        }

        HSVA color;
        if (color_rgba.a == 0)
            color = HSVA(0, 0, 0, 0);
        else
            color = color_rgba.operator HSVA();

        std::stringstream str;

        str << "HSVA(";
        str << "<tt>" << int(color.h * 1000) / 1000.f << "</tt>, ";
        str << "<tt>" << int(color.s * 1000) / 1000.f << "</tt>, ";
        str << "<tt>" << int(color.v * 1000) / 1000.f << "</tt>, ";
        str << "<tt>" << int(color.a * 1000) / 1000.f << "</tt>)";

        _current_color_label.set_text(str.str());
    }

    void LogBox::set_cursor_position(Vector2i position)
    {
        std::string x_sign = (position.x < 0 ? "-" : "");
        std::string y_sign = (position.y < 0 ? "-" : "");

        auto x_value = abs(position.x);
        auto y_value = abs(position.y);

        std::stringstream str;

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

    void LogBox::on_layer_resolution_changed()
    {
        set_layer_resolution(active_state->get_layer_resolution());
    }

    void LogBox::set_layer_resolution(Vector2ui size)
    {
        std::stringstream str;
        str << "[<tt>" << size.x << "</tt>, <tt>" << size.y << "</tt>]";
        _current_resolution_label.set_text(str.str());
    }
}
