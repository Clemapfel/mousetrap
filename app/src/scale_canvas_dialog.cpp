//
// Created by clem on 1/26/23.
//

#include <app/scale_canvas_dialog.hpp>
#include <app/project_state.hpp>
#include <app/config_files.hpp>

namespace mousetrap
{
    void ScaleCanvasDialog::set_scale_mode(ScaleMode mode)
    {
        auto max_size = state::settings_file->get_value_as<size_t>("global", "maximum_image_size");
        if (mode == ABSOLUTE)
        {
            _width_spin_button.set_upper_limit(max_size);
            _height_spin_button.set_upper_limit(max_size);

            if (mode == RELATIVE)
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
            _width_spin_button.set_upper_limit((max_size / active_state->get_layer_resolution().x) * 100);
            _height_spin_button.set_upper_limit((max_size / active_state->get_layer_resolution().y) * 100);

            if (mode == ABSOLUTE)
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
        auto target = _scale_mode == ABSOLUTE ? v : (v / 100) * active_state->get_layer_resolution().x;
        // TODO
    }

    void ScaleCanvasDialog::set_height(float v)
    {
        auto target = _scale_mode == ABSOLUTE ? v : (v / 100) * active_state->get_layer_resolution().y;
        // TODO
    }

    ScaleCanvasDialog::ScaleCanvasDialog()
        : _width_spin_button(1, 1, 1),
          _height_spin_button(1, 1, 1)
    {
        set_scale_mode(_scale_mode);

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

        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_button);
        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_label);

        _window_box.push_back(&_instruction_label);
        _window_box.push_back(&_spin_button_and_dropdown_box);
        _window_box.push_back(&_maintain_aspect_ratio_box);

        _dialog.get_content_area().push_back(&_window_box);
        _dialog.add_action_button("OK", [](ScaleCanvasDialog* instance){

            auto width_value = instance->_width_spin_button.get_value();
            auto height_value = instance->_height_spin_button.get_value();

            auto width = instance->_scale_mode == ABSOLUTE ? width_value  : (width_value / 100) * active_state->get_layer_resolution().x;
            auto height = instance->_scale_mode == ABSOLUTE ? height_value  : (height_value / 100) * active_state->get_layer_resolution().y;

            std::cout << "scale to: " << width << "x" << height << " (px)" << std::endl;

            instance->_dialog.close();
        }, this);

        _dialog.add_action_button("Cancel", [](ScaleCanvasDialog* instance){
            instance->_dialog.close();
        }, this);
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

