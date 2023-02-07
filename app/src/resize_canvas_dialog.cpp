//
// Created by clem on 2/5/23.
//

#include <app/resize_canvas_dialog.hpp>
#include <app/project_state.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    void ResizeCanvasDialog::on_layer_resolution_changed()
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

    void ResizeCanvasDialog::set_final_size(size_t w, size_t h)
    {
        _final_size_label.set_text("New Size: " + std::to_string(w) + " x " + std::to_string(h) + " px");
    }

    void ResizeCanvasDialog::set_x_offset(int x)
    {
        _x_offset = x;
        _x_offset_button.set_signal_value_changed_blocked(true);
        _x_offset_button.set_value(x);
        _x_offset_button.set_signal_value_changed_blocked(false);
    }

    void ResizeCanvasDialog::set_y_offset(int y)
    {
        _y_offset = y;
        _y_offset_button.set_signal_value_changed_blocked(true);
        _y_offset_button.set_value(y);
        _y_offset_button.set_signal_value_changed_blocked(false);
    }

    void ResizeCanvasDialog::center()
    {
        std::cerr << "[ERROR] In ResizeCanvasDialog::center: TODO" << std::endl;
    }

    void ResizeCanvasDialog::set_scale_mode(ScaleMode mode)
    {
        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        auto max_size = state::settings_file->get_value_as<size_t>("global", "maximum_image_size");
        if (mode == ABSOLUTE)
        {
            _width_spin_button.set_upper_limit(max_size);
            _height_spin_button.set_upper_limit(max_size);

            _width_spin_button.set_value(active_state->get_layer_resolution().x);
            _height_spin_button.set_value(active_state->get_layer_resolution().y);
        }
        else
        {
            _width_spin_button.set_upper_limit(int(max_size / float(active_state->get_layer_resolution().x)) * 100);
            _height_spin_button.set_upper_limit(int(max_size / float(active_state->get_layer_resolution().y)) * 100);

            _width_spin_button.set_value(100);
            _height_spin_button.set_value(100);
        }

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);

        _scale_mode = mode;
        set_final_size(active_state->get_layer_resolution().x, active_state->get_layer_resolution().y);
    }

    void ResizeCanvasDialog::set_aspect_ratio_locked(bool b)
    {
        _aspect_ratio_locked = b;
    }

    void ResizeCanvasDialog::set_width(float v)
    {
        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        size_t final_x = 0;
        size_t final_y = 0;

        if (_scale_mode == ABSOLUTE)
        {
            auto target = v;
            _width_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _height_spin_button.set_value(target * (active_state->get_layer_resolution().y / float(active_state->get_layer_resolution().x)));

            final_x = _width_spin_button.get_value();
            final_y = _height_spin_button.get_value();
        }
        else
        {
            auto target = v;
            _width_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _height_spin_button.set_value(v);

            final_x = _width_spin_button.get_value() / 100 *  active_state->get_layer_resolution().x;
            final_y = _height_spin_button.get_value() / 100 * active_state->get_layer_resolution().y;
        }

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);

        set_final_size(final_x, final_y);
    }

    void ResizeCanvasDialog::set_height(float v)
    {
        _width_spin_button.set_signal_value_changed_blocked(true);
        _height_spin_button.set_signal_value_changed_blocked(true);

        size_t final_x = 0;
        size_t final_y = 0;

        if (_scale_mode == ABSOLUTE)
        {
            auto target = v;
            _height_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _width_spin_button.set_value(target * (active_state->get_layer_resolution().x / float(active_state->get_layer_resolution().y)));

            final_x = _width_spin_button.get_value();
            final_y = _height_spin_button.get_value();
        }
        else
        {
            auto target = v;
            _height_spin_button.set_value(target);

            if (_aspect_ratio_locked)
                _width_spin_button.set_value(v);

            final_x = _width_spin_button.get_value() / 100 *  active_state->get_layer_resolution().x;
            final_y = _height_spin_button.get_value() / 100 * active_state->get_layer_resolution().y;
        }

        _width_spin_button.set_signal_value_changed_blocked(false);
        _height_spin_button.set_signal_value_changed_blocked(false);

        set_final_size(final_x, final_y);
    }

    ResizeCanvasDialog::operator Widget*()
    {
        return &_dialog;
    }

    void ResizeCanvasDialog::present()
    {
        _dialog.present();
    }

    ResizeCanvasDialog::ResizeCanvasDialog()
        : _width_spin_button(1, 1, 1),
          _height_spin_button(1, 1, 1),
          _x_offset_button(0, 0, 1),
          _y_offset_button(0, 0, 1)
    {
        set_scale_mode(_scale_mode);
        on_layer_resolution_changed();
        set_final_size(0, 0);

        _width_spin_button.connect_signal_value_changed([](SpinButton* button, ResizeCanvasDialog* instance) {
            instance->set_width(button->get_value());
        }, this);

        _height_spin_button.connect_signal_value_changed([](SpinButton* button, ResizeCanvasDialog* instance) {
            instance->set_height(button->get_value());
        }, this);

        _width_box.push_back(&_width_label);
        _width_box.push_back(&_width_spacer);
        _width_box.push_back(&_width_spin_button);

        _height_box.push_back(&_height_label);
        _height_box.push_back(&_height_spacer);
        _height_box.push_back(&_height_spin_button);

        for (auto* label : {
            &_width_label,
            &_height_label
        })
        {
            label->set_halign(GTK_ALIGN_START);
            label->set_expand(false);
            label->set_margin_end(state::margin_unit);
        }

        for (auto* spacer : {
            &_width_spacer,
            &_height_spacer
        })
        {
            spacer->set_opacity(0);
            spacer->set_expand(true);
            spacer->set_halign(GTK_ALIGN_CENTER);
        }

        for (auto* button : {
            &_width_spin_button,
            &_height_spin_button
        })
        {
            button->set_expand(false);
            button->set_halign(GTK_ALIGN_END);
        }

        _spin_button_box.push_back(&_width_box);
        _spin_button_box.push_back(&_height_box);
        _spin_button_box.set_expand(true);
        _spin_button_box.set_margin_vertical(state::margin_unit);

        _absolute_or_relative_dropdown.push_back(&_absolute_list_label, &_absolute_when_selected_label, [](ResizeCanvasDialog* instance){
            instance->set_scale_mode(ABSOLUTE);
        }, this);

        _absolute_or_relative_dropdown.push_back(&_relative_list_label, &_relative_when_selected_label, [](ResizeCanvasDialog* instance){
            instance->set_scale_mode(RELATIVE);
        }, this);

        _absolute_or_relative_dropdown.set_margin_start(state::margin_unit);
        _absolute_or_relative_dropdown.set_align(GTK_ALIGN_CENTER);
        _absolute_or_relative_dropdown.set_expand(false);

        _absolute_list_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "absolute_dropdown_label"));
        _absolute_when_selected_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "absolute_dropdown_label"));

        _relative_list_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "relative_dropdown_label"));
        _relative_when_selected_label.set_tooltip_text(state::tooltips_file->get_value("scale_canvas_dialog", "relative_dropdown_label"));

        _spin_button_and_dropdown_box.push_back(&_spin_button_box);
        _spin_button_and_dropdown_box.push_back(&_absolute_or_relative_dropdown);

        _maintain_aspect_ratio_button.set_active(_aspect_ratio_locked);
        _maintain_aspect_ratio_button.connect_signal_toggled([](CheckButton* button, ResizeCanvasDialog* instance){
            instance->set_aspect_ratio_locked(button->get_active());
        }, this);
        _maintain_aspect_ratio_button.set_margin_horizontal(state::margin_unit);

        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_button);
        _maintain_aspect_ratio_box.push_back(&_maintain_aspect_ratio_label);

        _final_size_label.set_justify_mode(JustifyMode::LEFT);
        _final_size_label.set_halign(GTK_ALIGN_START);
        _final_size_label.set_margin_top(state::margin_unit);

        _spin_button_and_dropdown_box.set_margin_start(state::margin_unit);
        _maintain_aspect_ratio_box.set_margin_start(state::margin_unit);

        _offset_label.set_halign(GTK_ALIGN_START);

        for (auto* label : {&_x_offset_label, &_y_offset_label})
        {
            label->set_halign(GTK_ALIGN_START);
            label->set_margin_end(state::margin_unit * 2);
        }

        for (auto* box : {&_x_offset_box, &_y_offset_box})
        {
            box->set_margin_horizontal(state::margin_unit * 2);
        }

        _x_offset_button.connect_signal_value_changed([](SpinButton* scale, ResizeCanvasDialog* instance){
            instance->set_x_offset(scale->get_value());
        }, this);

        _y_offset_button.connect_signal_value_changed([](SpinButton* scale, ResizeCanvasDialog* instance){
            instance->set_x_offset(scale->get_value());
        }, this);

        _x_offset_box.push_back(&_x_offset_label);
        _x_offset_box.push_back(&_x_offset_button);
        _y_offset_box.push_back(&_y_offset_label);
        _y_offset_box.push_back(&_y_offset_button);

        _center_button.set_child(&_center_button_label);
        _center_button.connect_signal_clicked([](Button* button, ResizeCanvasDialog* instance){
            instance->center();
        }, this);
        _center_button.set_valign(GTK_ALIGN_CENTER);

        _offset_box.push_back(&_x_offset_box);
        _offset_box.push_back(&_y_offset_box);
        _offset_and_center_box.push_back(&_offset_box);
        _offset_and_center_box.push_back(&_center_button);

        _window_box.push_back(&_instruction_label);
        _window_box.push_back(&_spin_button_and_dropdown_box);
        _window_box.push_back(&_maintain_aspect_ratio_box);
        _window_box.push_back(&_final_size_label);

        {
            auto* spacer = new SeparatorLine();
            spacer->set_size_request({0, 3});
            spacer->set_vexpand(false);
            spacer->set_margin_top(state::margin_unit);
            _window_box.push_back(spacer);
        }

        _window_box.push_back(&_offset_label);
        _window_box.push_back(&_offset_and_center_box);

        {
            auto* spacer = new SeparatorLine();
            spacer->set_size_request({0, 3});
            spacer->set_vexpand(false);
            spacer->set_margin_top(state::margin_unit);
            _window_box.push_back(spacer);
        }

        _instruction_label.set_halign(GTK_ALIGN_START);

        for (auto* label : {&_offset_label})
            label->set_margin_vertical(state::margin_unit);

        _window_box.set_margin(state::margin_unit);

        _accept_button.set_child(&_accept_button_label);
        _cancel_button.set_child(&_cancel_button_label);

        _dialog.get_content_area().push_back(&_window_box);

        _dialog.add_action_widget(&_cancel_button, [](ResizeCanvasDialog* instance){
            instance->_dialog.close();
        }, this);

        _dialog.add_action_widget(&_accept_button,[](ResizeCanvasDialog* instance){

            auto width_value = instance->_width_spin_button.get_value();
            auto height_value = instance->_height_spin_button.get_value();

            auto width = instance->_scale_mode == ABSOLUTE ? width_value  : (width_value / 100) * active_state->get_layer_resolution().x;
            auto height = instance->_scale_mode == ABSOLUTE ? height_value  : (height_value / 100) * active_state->get_layer_resolution().y;

            active_state->resize_canvas(
                Vector2ui(round(width), round(height)),
                Vector2i(instance->_x_offset, instance->_y_offset)
            );

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

        state::actions::resize_canvas_dialog_open.set_function([](){
            state::resize_canvas_dialog->present();
        });
        state::add_shortcut_action(state::actions::resize_canvas_dialog_open);
    }
}
