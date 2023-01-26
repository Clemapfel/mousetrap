//
// Created by clem on 1/26/23.
//

#include <app/scale_canvas_dialog.hpp>
#include <app/project_state.hpp>
#include <app/config_files.hpp>

#include <boost/rational.hpp>

namespace mousetrap
{
    void ScaleCanvasDialog::on_layer_resolution_changed()
    {
        auto size = active_state->get_layer_resolution();
        if (_scale_mode == ABSOLUTE)
        {
            _width_spin_button.set_value(size.x);
            _height_spin_button.set_value(size.y);
        }

        auto ratio = size.x / float(size.y);
        auto ratio_string =  std::to_string(ratio);
        std::string ratio_string_formatted = "";

        if (int(ratio) == ratio)
            ratio_string_formatted = std::to_string(int(ratio));
        else
        {
            bool comma_seen = false;
            size_t post_comma_count = 0;
            for (auto c:ratio_string)
            {
                if (comma_seen and (post_comma_count++ >= 3 or c == '0'))
                    break;

                ratio_string_formatted.push_back(c);

                if (c == '.')
                    comma_seen = true;
            }
        }

        _maintain_aspect_ratio_label.set_text(state::tooltips_file->get_value("scale_canvas_dialog", "maintain_aspect_ratio_label") + " (" + ratio_string_formatted + ")");
    }

    void ScaleCanvasDialog::set_scale_mode(ScaleMode mode)
    {
        auto max_size = state::settings_file->get_value_as<size_t>("global", "maximum_image_size");
        if (mode == ABSOLUTE)
        {
            _width_spin_button.set_upper_limit(max_size);
            _height_spin_button.set_upper_limit(max_size);

            if (false) //(mode == RELATIVE)
            {
                auto width_before = _width_spin_button.get_value();
                _width_spin_button.set_value(width_before * active_state->get_layer_resolution().x);

                auto height_before = _height_spin_button.get_value();
                _height_spin_button.set_value(height_before * active_state->get_layer_resolution().y);
            }
            else
            {
                _width_spin_button.set_value(active_state->get_layer_resolution().x);
                _height_spin_button.set_value(active_state->get_layer_resolution().y);
            }
        }
        else
        {
            _width_spin_button.set_upper_limit((max_size / float(active_state->get_layer_resolution().x)) * 100);
            _height_spin_button.set_upper_limit((max_size / float(active_state->get_layer_resolution().y)) * 100);

            if (false) //(mode == ABSOLUTE)
            {
                auto width_before = _width_spin_button.get_value();
                _width_spin_button.set_value(width_before / active_state->get_layer_resolution().x * 100);

                auto height_before = _height_spin_button.get_value();
                _height_spin_button.set_value(height_before / active_state->get_layer_resolution().y * 100);
            }
            else
            {
                _width_spin_button.set_value(100);
                _height_spin_button.set_value(100);
            }
        }

        _scale_mode = mode;
    }

    void ScaleCanvasDialog::set_aspect_ratio_locked(bool b)
    {
        _aspect_ratio_locked = b;
        // TODO
    }

    void ScaleCanvasDialog::set_width(float v)
    {
        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        auto target = _scale_mode == ABSOLUTE ? v : (v / 100) * active_state->get_layer_resolution().x;
        _width_spin_button.set_value(target);

        auto ratio = active_state->get_layer_resolution().y / float(active_state->get_layer_resolution().x);

        if (_aspect_ratio_locked)
            if (_scale_mode == ABSOLUTE)
                _height_spin_button.set_value(target * ratio);
            else
                _height_spin_button.set_value((target * ratio) / active_state->get_layer_resolution().y);

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);
    }

    void ScaleCanvasDialog::set_height(float v)
    {
        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);
    }

    ScaleCanvasDialog::ScaleCanvasDialog()
        : _width_spin_button(1, 1, 1),
          _height_spin_button(1, 1, 1)
    {
        set_scale_mode(_scale_mode);
        on_layer_resolution_changed();

        _width_spin_button.connect_signal_value_changed([](SpinButton* button, ScaleCanvasDialog* instance) {
            instance->set_width(button->get_value());
        }, this);

        _height_spin_button.connect_signal_value_changed([](SpinButton* button, ScaleCanvasDialog* instance) {
            instance->set_height(button->get_value());
        }, this);

        _width_box.push_back(&_width_label);
        _width_box.push_back(&_width_spacer);
        _width_box.push_back(&_width_spin_button);

        _height_box.push_back(&_height_label);
        _height_box.push_back(&_height_spacer);
        _height_box.push_back(&_height_spin_button);

        for (auto* label : {&_width_label, &_height_label})
        {
            label->set_halign(GTK_ALIGN_START);
            label->set_expand(false);
        }

        for (auto* spacer : {&_width_spacer, &_height_spacer})
        {
            spacer->set_opacity(0);
            spacer->set_expand(true);
            spacer->set_halign(GTK_ALIGN_CENTER);
        }

        for (auto* button : {&_width_spin_button, &_height_spin_button})
        {
            button->set_expand(false);
            button->set_halign(GTK_ALIGN_END);
        }

        _spin_button_box.push_back(&_width_box);
        _spin_button_box.push_back(&_height_box);
        _spin_button_box.set_expand(true);
        _spin_button_box.set_margin_vertical(state::margin_unit);

        _absolute_or_relative_dropdown.push_back(&_absolute_list_label, &_absolute_when_selected_label, [](ScaleCanvasDialog* instance){
            instance->set_scale_mode(ABSOLUTE);
        }, this);

        _absolute_or_relative_dropdown.push_back(&_relative_list_label, &_relative_when_selected_label, [](ScaleCanvasDialog* instance){
            instance->set_scale_mode(RELATIVE);
        }, this);

        _absolute_or_relative_dropdown.set_margin_start(state::margin_unit);
        _absolute_or_relative_dropdown.set_align(GTK_ALIGN_CENTER);
        _absolute_or_relative_dropdown.set_expand(false);

        _spin_button_and_dropdown_box.push_back(&_spin_button_box);
        _spin_button_and_dropdown_box.push_back(&_absolute_or_relative_dropdown);

        _maintain_aspect_ratio_button.set_active(_aspect_ratio_locked);
        _maintain_aspect_ratio_button.connect_signal_toggled([](CheckButton* button, ScaleCanvasDialog* instance){
            instance->set_aspect_ratio_locked(button->get_active());
        }, this);
        _maintain_aspect_ratio_button.set_margin_horizontal(state::margin_unit);

        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_button);
        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_label);

        _window_box.push_back(&_instruction_label);
        _window_box.push_back(&_spin_button_and_dropdown_box);
        _window_box.push_back(&_maintain_aspect_ratio_box);

        _instruction_label.set_justify_mode(JustifyMode::LEFT);
        _instruction_label.set_halign(GTK_ALIGN_START);
        _window_box.set_margin(state::margin_unit);

        _accept_button.set_child(&_accept_button_label);
        _cancel_button.set_child(&_cancel_button_label);

        _dialog.get_content_area().push_back(&_window_box);

        _dialog.add_action_widget(&_cancel_button, [](ScaleCanvasDialog* instance){
            instance->_dialog.close();
        }, this);

        _dialog.add_action_widget(&_accept_button,[](ScaleCanvasDialog* instance){

            auto width_value = instance->_width_spin_button.get_value();
            auto height_value = instance->_height_spin_button.get_value();

            auto width = instance->_scale_mode == ABSOLUTE ? width_value  : (width_value / 100) * active_state->get_layer_resolution().x;
            auto height = instance->_scale_mode == ABSOLUTE ? height_value  : (height_value / 100) * active_state->get_layer_resolution().y;

            std::cout << "scale to: " << width << "x" << height << " (px)" << std::endl;

            instance->_dialog.close();
        }, this);

        auto button_size = std::max<float>(_accept_button.get_preferred_size().natural_size.x, _cancel_button.get_preferred_size().natural_size.x);
        for (auto* button : {&_accept_button, &_cancel_button})
        {
            button->set_margin_end(state::margin_unit);
            button->set_size_request({button_size, 0});
        }

        _accept_button.set_margin_end(state::margin_unit);
        _cancel_button.set_margin_end(state::margin_unit);
        GtkWidget* parent = gtk_widget_get_parent(_accept_button.operator GtkWidget*());
        gtk_widget_set_margin_start(parent, state::margin_unit);
        gtk_widget_set_margin_bottom(parent, state::margin_unit);
    }

    ScaleCanvasDialog::operator Widget*()
    {
        return &_dialog;
    }

    void ScaleCanvasDialog::present()
    {
        _dialog.present();
    }
}

